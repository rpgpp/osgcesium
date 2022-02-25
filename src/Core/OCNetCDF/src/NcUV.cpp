#include "NcUV.h"

using namespace OC;
using namespace OC::Volume;

NcUV::NcUV()
{
    mClampMin = -70.0;
    mClampMax = 70.0;
}

NcUV::~NcUV()
{
}

CVector3i NcUV::getVarDim(String name)
{
    CVector3i dim = CVector3i::ZERO;
    int varID;
    int e = nc_inq_varid(_ncid, name.c_str(), &varID);
    if (NC_NOERR == e)
    {
        int ndimsp = 0;
        nc_inq_varndims(_ncid, varID, &ndimsp);
        if (ndimsp < 3)
        {
            return dim;
        }
        int dimids[NC_MAX_VAR_DIMS];
        nc_inq_vardimid(_ncid, varID, &dimids[0]);
        for (int i = 0; i < ndimsp; i++)
        {
            char dname[128];
            long d;
            ncdiminq(_ncid, dimids[i], dname, &d);
            if (String(dname) == levelX)
            {
                dim.x = d;
            }
            else if (String(dname) == levelY)
            {
                dim.y = d;
            }
            else if (String(dname) == levelZ && levelZ != "")
            {
                dim.z = d;
            }
        }
    }
    return dim;
}

inline bool NcUV::isValid()
{
    return nameU != ""
        && nameV != ""
        && levelX != ""
        && levelY != "";
}

inline int NcUV::getLevel()
{
    return levelZ == "" ? 2 : 3;
}

bool NcUV::convert(std::string filename)
{
    if (!isValid())
    {
        return NULL;
    }

    NcCheck(nc_open(filename.c_str(), NC_NOWRITE, &_ncid));
    int ncid = _ncid;

    int level = getLevel();

    CVector3i dim1 = getVarDim(nameU);
    CVector3i dim2 = getVarDim(nameV);

    if (dim1 != dim2 || dim1 == CVector3i::ZERO)
    {
        return NULL;
    }

    long xSize, ySize, zSize;
    long offsetYSize;

    if (level == 2)
    {
        zSize = 1;
        ySize = dim1.y;
        xSize = dim1.x;
        offsetYSize = 0;
    }
    else
    {
        zSize = dim1.z;
        ySize = xSize = dim1.y * dim1.x;
        offsetYSize = dim1.x;
    }

    long xySize = dim1.y * dim1.x;
    long xyzSize = zSize * xySize;
    auto getData = [ncid](String name, void* ip)
    {
        int varID;
        nc_inq_varid(ncid, name.c_str(), &varID);
        nc_get_var(ncid, varID, ip);
    };

    var1Values.resize(xyzSize);
    var2Values.resize(xyzSize);

    lonValues.resize(xSize);
    latValues.resize(ySize);

    getData("lon", &lonValues[0]);
    getData("lat", &latValues[0]);
    getData(nameU, &var1Values[0]);
    getData(nameV, &var2Values[0]);

    float maxLevel = 0.0;
    int imageR = 1;
    if (level == 3)
    {
        heiValues.resize(zSize);
        getData(levelZ, &heiValues[0]);
        maxLevel = heiValues[zSize - 1];
        imageR = maxLevel / interval + 1;
    }

    int imageS = dim1.x;
    int imageT = dim1.y;

    CRectangle extent;

    osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);

    long offset3 = 0;
    for (long z = 0; z < imageR; z++)
    {
        long offsetY = 0;
        for (long y = 0; y < imageT; y++)
        {
            long offsetX = 0;
            for (long x = 0; x < imageS; x++)
            {
                if (z == 0)
                {
                    if (level == 2)
                    {

                        float lon = lonValues[offsetY * offsetYSize + offsetX];
                        float lat = latValues[offsetY];
                        extent.merge(CVector2(lon, lat));
                    }
                    else
                    {
                        float lon = lonValues[offsetY * offsetYSize + offsetX];
                        float lat = latValues[offsetY * offsetYSize + offsetX];
                        extent.merge(CVector2(lon, lat));
                    }
                    offsetX++;
                }

                float v1 = var1Values[offset3];
                float v2 = var2Values[offset3];
                PRC1(v1);
                PRC1(v2);

                osg::Vec4ub color1 = _EncodeValue(v1);
                osg::Vec4ub color2 = _EncodeValue(v2);
                unsigned char* data = (unsigned char*)image->data(x, y, z);
                data[0] = color1.r();
                data[1] = color1.g();
                data[2] = color2.r();
                data[3] = color2.g();

                offset3++;
            }
            offsetY++;
        }
    }

    float degreeWidth_bottom = extent.getWidth();
    float degreeHeight = extent.getHeight();
    float w_meta1 = width_in_meter(extent.getMinimum().y + degreeHeight * 0.5, degreeWidth_bottom);
    float h_meta1 = height_in_meter(degreeHeight);
    float d_meta1 = maxLevel;

    CVector2 center = extent.getCenter();

    setExtent(extent);
    setPosition(center);
    setMeter(w_meta1, h_meta1, d_meta1);
    setUint8Image(image);

    return true;
}
