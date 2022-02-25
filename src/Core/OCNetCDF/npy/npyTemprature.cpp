#include "npyTemprature.h"
#include "npy/cnpy.h"

using namespace cnpy;

namespace OC
{
    namespace Volume
    {
        npyTemprature::npyTemprature(String varname)
            :mVarname(varname)
        {
            mClampMin = -20.0;
            mClampMax = 60.0;
        }

        npyTemprature::~npyTemprature()
        {

        }

        bool npyTemprature::convert(std::string filename)
        {
            int imageT = 360;
            int imageS = 480;
            int imageR = 26;

            bool isTemp = false;
            bool isWind = false;
            if (StringUtil::startsWith(osgDB::getSimpleFileName(filename), "specific_humidity"))
            {
                mClampMin = 0.0;
                imageR = 21;
            }
            else if (StringUtil::startsWith(osgDB::getSimpleFileName(filename), "air_temperature"))
            {
                isTemp = true;
            }
            else if (StringUtil::startsWith(osgDB::getSimpleFileName(filename), "eastward_wind"))
            {
                isWind = true;
                mClampMin = -70.0;
                mClampMax = 70.0;
            }

            String dir = osgDB::getFilePath(filename);

            NpyArray heiArray = npy_load(dir + "/height.npy");
            NpyArray lonArray = npy_load(dir + "/longitude.npy");
            NpyArray latArray = npy_load(dir + "/latitude.npy");
            NpyArray tempArray = npy_load(filename);

            int64* heidata = heiArray.data<int64>();
            double* londata = lonArray.data<double>();
            double* latdata = latArray.data<double>();
            float* tempdata = tempArray.data<float>();
            float* tempdata2 = NULL;
            NpyArray tempArray2;
            if (isWind)
            {
                tempArray2 = npy_load(StringUtil::replaceAll(filename, "eastward_wind", "northward_wind"));
                tempdata2 = tempArray2.data<float>();
            }


            osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);

            float minV = 1e+10;
            float maxV = -1e+10;

            for (long i = 0; i < tempArray.num_vals; i++)
            {
                float v = tempdata[i];
                if (isTemp) v -= 273.15;
                minV = min(minV, v);
                maxV = max(maxV, v);
                if (tempdata2)
                {
                    v = tempdata2[i];
                    minV = min(minV, v);
                    maxV = max(maxV, v);
                }
            }

            CRectangle rectangle;
            double d_meters = 0.0;
            long offsetXYZ = 0;
            for (int z = 0; z < imageR; z++)
            {
                long offsetXY = 0;
                for (int y = 0; y < imageT; y++)
                {
                    for (int x = 0; x < imageS; x++, offsetXY++, offsetXYZ++)
                    {
                        if (z == 0)
                        {
                            rectangle.merge(CVector2(londata[offsetXY], latdata[offsetXY]));
                        }

                        float v = tempdata[offsetXYZ];
                        osg::Vec4ub color;
                        if (isTemp)
                        {
                            v -= 273.15;
                            PRC1(v);
                            color = _EncodeValue(v);
                        }
                        else if (isWind)
                        {
                            float v2 = tempdata2[offsetXYZ];
                            osg::Vec4ub color1 = _EncodeValue(v);
                            osg::Vec4ub color2 = _EncodeValue(v2);
                            color.r() = color1.r();
                            color.g() = color1.g();
                            color.b() = color2.r();
                            color.a() = color2.g();
                        }
                        else
                        {
                            v = (v - minV) / (maxV - minV);
                            PRC1(v);
                            color = _EncodeValue(v);
                        }

                        unsigned char* data = (unsigned char*)image->data(x, y, z);
                        data[0] = color.r();
                        data[1] = color.g();
                        data[2] = color.b();
                        data[3] = color.a();

                        if (!isWind)
                        {
                            osg::Vec2 normal2D(0.0, 0.0);
                            if (y > 0 && x > 0 && y < imageT - 1 && x < imageS - 1)
                            {
                                float px = v + (tempdata[offsetXYZ + 1] - v) / imageS;
                                float nx = v - (v - tempdata[offsetXYZ - 1]) / imageS;
                                float py = v + (tempdata[offsetXYZ + imageS] - v) / imageT;
                                float ny = v - (v - tempdata[offsetXYZ - imageS]) / imageT;
                                if (px < mClampMin) px = mClampMin;
                                if (nx < mClampMin) nx = mClampMin;
                                if (py < mClampMin) py = mClampMin;
                                if (ny < mClampMin) ny = mClampMin;
                                float dx = px - nx;
                                float dy = py - ny;
                                if (dx != 0.0 || dy != 0.0)
                                {
                                    normal2D = osg::Vec2(dx, dy);
                                    normal2D.normalize();
                                    normal2D *= 0.5;
                                    PRC1(normal2D.x());
                                    PRC1(normal2D.y());
                                    normal2D += osg::Vec2(0.5, 0.5);
                                }
                                data[2] = normal2D.x() * 255.0;
                                data[3] = normal2D.y() * 255.0;
                            }
                        }
                    }
                }

                d_meters = max(d_meters, heidata[z]);
            }


            float w_meters = width_in_meter(rectangle.getMinimum().y, rectangle.getWidth());
            float h_meters = height_in_meter(rectangle.getHeight());

            setPosition(rectangle.getCenter());
            setMeter(w_meters, h_meters, d_meters);
            setExtent(rectangle);

            setUint8Image(image);

            return true;
        }
    }
}