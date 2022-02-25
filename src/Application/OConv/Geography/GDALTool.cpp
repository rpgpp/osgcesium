#include "GDALTool.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "OCMain/DataStream.h"

namespace OC
{
	GDALTool::GDALTool()
	{
        OGRRegisterAll();
        GDALAllRegister();
        CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	}

	GDALTool::~GDALTool()
	{

	}

    void GDALTool::testIDW(String filename)
    {
        std::ifstream ifs(filename.c_str());
        FileStreamDataStream dataStream(&ifs,false);
    }

	void GDALTool::testContour(String inputDEM, String outputSHP, double dfContourInterval)
	{
        GDALDataset* pInDataset = (GDALDataset*)GDALOpen(inputDEM.c_str(), GA_ReadOnly);
        if (pInDataset == NULL)
        {
            printf("读取图像失败！");
            return;
        }

        int nDemWidth = pInDataset->GetRasterXSize(); //获取图像宽  
        int nDemHeight = pInDataset->GetRasterYSize(); //获取图像高  
        int Count = pInDataset->GetRasterCount(); //波段数  


        //读取图像数据波段
        GDALRasterBand* pInRasterBand = pInDataset->GetRasterBand(1);
        float* pData = new float[nDemWidth * nDemHeight]();

        CPLErr err = pInRasterBand->RasterIO(GF_Read, 0, 0, nDemWidth, nDemHeight, pData, nDemWidth, nDemHeight, GDT_Float32, 0, 0);
        if (err == CE_Failure)
        {
            cout << "读取DEM图像数据时出错！" << endl;
            GDALDestroyDriverManager();
            return;
        }

        //判断图像中是否有异常值，并获取异常值实际值
        float fNoData = 0;
        int nIdx;
        for (int i = 0; i < nDemHeight; i++)
        {
            for (int j = 0; j < nDemWidth; j++)
            {
                nIdx = i * nDemWidth + j;
                if (pData[nIdx] <= -9999)
                {
                    fNoData = pData[nIdx];
                }
            }
        }

        //创建矢量图
        const char* pszDriverName = "ESRI Shapefile";
        GDALDriver* poDriver;
        GDALAllRegister();
        poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        if (poDriver == NULL)
        {
            printf("%s driver not available.\n", pszDriverName);
            exit(1);
        }
        GDALDataset* poDS;
        poDS = poDriver->Create(outputSHP.c_str(), 0, 0, 0, GDT_Unknown, NULL); //创建数据源
        if (poDS == NULL)
        {
            printf("Creation of output file failed.\n");
            exit(1);
        }
        OGRLayer* poLayer;
        OGRSpatialReference* poSpatialRef = new OGRSpatialReference(pInDataset->GetProjectionRef());
        poLayer = poDS->CreateLayer("Elevation", poSpatialRef, wkbLineString, NULL); //创建图层
        if (poLayer == NULL)
        {
            printf("Layer creation failed.\n");
            exit(1);
        }


        OGRFieldDefn ofieldDef1("Elevation", OFTInteger);    //在矢量图中创建高程值字段
        if (poLayer->CreateField(&ofieldDef1) != OGRERR_NONE)
        {
            cout << "创建矢量图层属性表失败！" << endl;
            GDALClose((GDALDatasetH)poDS);
            GDALClose(pInDataset);
            return;
        }

        //根据图像波段生成矢量图等高线
        if (fNoData == 0)
            GDALContourGenerate(pInRasterBand, dfContourInterval, 0, 0, NULL, false, 0, (OGRLayerH)poLayer, -1, 0, NULL, NULL);
        else //有异常值时，不对异常值进行处理
            GDALContourGenerate(pInRasterBand, dfContourInterval, 0, 0, NULL, true, fNoData, (OGRLayerH)poLayer, -1, 0, NULL, NULL);

        GDALClose(poDS);
        GDALClose(pInDataset);


        if (pData != NULL)
        {
            delete[] pData;
            pData = NULL;
        }
	}
}