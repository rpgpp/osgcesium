#include "NcepWind.h"
#include "OCMain/osgearth.h"
#include "gdal_priv.h"
#include "gdalwarper.h"
#include <ogr_spatialref.h>
#include "OCMain/DataStream.h"

namespace OC
{
	NcepWind::NcepWind()
	{
        mClampMin = -70.0;
        mClampMax = 70.0;
	}

	NcepWind::~NcepWind()
	{

	}

    auto readMeta(GDALDataset* dataSet, GDALRasterBand* band)
    {
        StringMap  meta;
        CSLConstList names1 = band->GetMetadataDomainList();
        for (int i = 0; names1 && names1[i]; i++)
        {
            String domain = names1[i];
            CSLConstList values = band->GetMetadata(domain.c_str());
            for (int j = 0; values && values[j]; j++)
            {
                String val = values[j];
                StringVector sv = StringUtil::split(val, "=");
                if (sv.size() == 2)
                {
                    String k = sv[0];
                    String v = sv[1];
                    StringUtil::trim(k);
                    StringUtil::trim(v);
                    meta[k] = v;
                }
                else
                {
                    meta[val] = val;
                }
            }
        }
        return meta;
    };

	void NcepWind::readGrib(String filename)
	{
        OGRRegisterAll();
        GDALDataset* dataSet = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);

        if (dataSet == NULL)
        {
            return;
        }
        int bandCount = dataSet->GetRasterCount();
        if (bandCount != 2)
        {
            GDALClose(dataSet);
            return;
        }

        double adfGeoTransform[6];
        dataSet->GetGeoTransform(&adfGeoTransform[0]);

        int dimX = GDALGetRasterXSize(dataSet);
        int dimY = GDALGetRasterYSize(dataSet);

        CVector2 origin(adfGeoTransform[0], adfGeoTransform[3]);
        CVector2 pixelOffset(adfGeoTransform[1], adfGeoTransform[5]);

        CVector2 endFrame;
        endFrame.x = origin.x + pixelOffset.x * dimX;
        endFrame.y = origin.y + pixelOffset.y * dimY;

        CRectangle rect;
        rect.merge(origin);
        rect.merge(endFrame);

        GDALRasterBand* bandU = dataSet->GetRasterBand(1);
        GDALRasterBand* bandV = dataSet->GetRasterBand(2);

        StringMap meta1 = readMeta(dataSet, bandU);
        StringMap meta2 = readMeta(dataSet, bandV);
        if (StringUtil::startsWith(meta1["GRIB_COMMENT"], "v-component"))
        {
            GDALRasterBand* tmp = bandU;
            bandU = bandV;
            bandV = tmp;
        }

        int blockSizeX, blockSizeY;
        bandU->GetBlockSize(&blockSizeX, &blockSizeY);
        int nXBlocks = (dimX + blockSizeX - 1) / blockSizeX;
        int nYBlocks = (dimY + blockSizeY - 1) / blockSizeY;
        //bandU->ReadBlock();

        String datatype = GDALGetDataTypeName(bandU->GetRasterDataType());
        dimX--;
        dimY--;

        long size = dimX * dimY;
        MemoryDataStreamPtr dataStreamU(new MemoryDataStream(new double[size], size));
        MemoryDataStreamPtr dataStreamV(new MemoryDataStream(new double[size], size));
        bandU->RasterIO(GF_Read, 1, 1, dimX, dimY, dataStreamU->getPtr(), dimX, dimY, GDT_Float64, 0, 0);
        bandV->RasterIO(GF_Read, 1, 1, dimX, dimY, dataStreamV->getPtr(), dimX, dimY, GDT_Float64, 0, 0);

        osg::ref_ptr<osg::Image> image = _CreateVolumeImage(dimX, dimY, 1);

        long flipOffset = 0;

        if (flipX) {
            flipOffset = dimX * 0.5;
        }

        long offset = 0;
        for (long y = 0; y < dimY; y++)
        {
            for (long x = 0; x < dimX; x++)
            {
                float v1 = ((double*)dataStreamU->getPtr())[offset];
                float v2 = ((double*)dataStreamV->getPtr())[offset];
                osg::Vec4ub color1 = _EncodeValue(v1);
                osg::Vec4ub color2 = _EncodeValue(v2);
                unsigned char* data = (unsigned char*)image->data((x + flipOffset) % dimX, y, 0);
                data[0] = color1.r();
                data[1] = color1.g();
                data[2] = color2.r();
                data[3] = color2.g();
                offset++;
            }
        }

        CVector2 center = rect.getCenter();
        setExtent(rect);
        setPosition(osg::Vec3d(center.x, center.y, 0.0));
        setUint8Image(image);

        GDALClose(dataSet);
	}

	bool NcepWind::convert(String filename)
	{
		if (!FileUtil::FileExist(filename))
		{
			String name, ext, dir;
			StringUtil::splitFullFilename(filename, name, ext, dir);

			StringVector sv = StringUtil::split(name, "_");
			if (sv.size() == 3)
			{
				static String RES = "0p25"; //0p25, 0p50 or 1p00
				static String BBOX = "leftlon=0&rightlon=360&toplat=90&bottomlat=-90";
				static String LEVEL = "lev_10_m_above_ground=on";
				static String gfs_url = "https://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_${RES}.pl?file=gfs.t${GFS_TIME}z.pgrb2.${RES}.f000&${LEVEL}&subregion=&${BBOX}&dir=%2Fgfs.${GFS_DATE}%2F${GFS_TIME}%2Fatmos";
				String gfs_date = sv[1];
				String gfs_time = sv[2];
				String dataUrl = StringUtil::replaceAll(gfs_url, "${RES}", RES);
				dataUrl = StringUtil::replaceAll(dataUrl, "${BBOX}", BBOX);
				dataUrl = StringUtil::replaceAll(dataUrl, "${LEVEL}", LEVEL);
				dataUrl = StringUtil::replaceAll(dataUrl, "${GFS_TIME}", gfs_time);
				dataUrl = StringUtil::replaceAll(dataUrl, "${GFS_DATE}", gfs_date);
				dataUrl += "&var_UGRD=on&var_VGRD=on";
				if (!osgEarth::HTTPClient::download(dataUrl, filename))
				{
					std::cout << "error in download:" << dataUrl << std::endl;
				}
			}
		}

        readGrib(filename);

		return true;
	}

}