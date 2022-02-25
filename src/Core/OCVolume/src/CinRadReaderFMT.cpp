#include "CinRadReaderFMT.h"

namespace OC
{
    namespace Volume
    {
        inline float decodeDBZ(CINRAD_DataFMT t, int i)
        {
            uint8 code = t.bufferDBZ[i];
            int scale = t.Moment.scale;
            int offset = t.Moment.offset;
            float v = (code < 5) ? 0.0 : (code - offset) / scale;
            PRC1(v);
            return v;
        }

        CinRadReaderFMT::CinRadReaderFMT()
        {
            mUnit = 1000.0;
            mRadius = 460;
            mClampMin = 0.0;
            mClampMax = 70.0;
        }

        CinRadReaderFMT::~CinRadReaderFMT()
        {

        }

        void CinRadReaderFMT::parseData(std::istream& sstream)
        {
            sstream.seekg(0);
            CINRAD_RecordFMT record;
            std::istream& is = sstream.read((char*)&record, sizeof(CINRAD_RecordFMT));
            if (!is)
            {
                return;
            }

            int cutNumber = record.TaskConfig.cutNumber;
            if (cutNumber < 1 || cutNumber > 33)
            {
                return;
            }

            double longitude = record.SiteConfig.longitude;
            double latitude = record.SiteConfig.latitude;
            setPosition(osg::Vec3d(longitude,latitude,0.0));

            std::vector<CINRAD_RecordFMT_CutConfig> headerConfigs;
            headerConfigs.resize(cutNumber);
            for (int i = 0; i < cutNumber; i++)
            {
                sstream.read((char*)&headerConfigs[i], sizeof(CINRAD_RecordFMT_CutConfig));
            }

            mUnit = headerConfigs[0].logResolution;

            CINRAD_DataFMT cinData;
            while (sstream.read((char*)&cinData, sizeof(CINRAD_RadialDataFMT)))
            {
                for (int i = 0; i < cinData.Radial.momentNumber; i++)
                {
                    if (!sstream.read((char*)&cinData.Moment, sizeof(CINRAD_MomentDataFMT)))
                    {
                        return;
                    }

                    int elevNo = cinData.Radial.elevationNumber;
                    if (cinData.Moment.dataType == 2)// DBZ
                    {
                        if (cinData.Moment.length > mRadius)
                        {
                            mRadius = cinData.Moment.length;
                        }
                        cinData.bufferDBZ.resize(cinData.Moment.length);
                        if (!sstream.read((char*)&(cinData.bufferDBZ[0]), cinData.Moment.length))
                        {
                            return;
                        }
                        mPitchRadialMapDBZ[elevNo].push_back(cinData);
                    }
                    else
                    {
                        int count = cinData.Moment.length;
                        sstream.clear(); //Clear fail status in case eof was set
                        sstream.seekg(static_cast<std::istream::pos_type>(count), std::ios::cur);
                    }
                }
            }
        }

        void CinRadReaderFMT::generateImage()
        {
            int imageS = mRadius * 2;
            int imageT = mRadius * 2;

            static float stepRadian = osg::DegreesToRadians(0.9836);
            TListMap::iterator it = mPitchRadialMapDBZ.begin();
            for (; it != mPitchRadialMapDBZ.end(); it++)
            {
                TList& list = it->second;
                float piching = list[0].Radial.elevation;
                mPitchList.push_back(piching);
                piching = osg::DegreesToRadians(piching);
                osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, 1);
                osg::ref_ptr<osg::Image> floatImage = _CreateVolumeImage(imageS, imageT, 1, GL_RED, GL_FLOAT);

                TList::iterator record = list.begin();
                for (;record!=list.end();record++)
                {
                    float heading = record->Radial.azimunth;
                    heading = osg::DegreesToRadians(heading);
                    for (int i = 0; i < record->Moment.length; i++)
                    {
                        float v = decodeDBZ(*record, i);

                        if (v <= 0.0)
                        {
                            continue;
                        }

                        osg::Vec4ub color = _EncodeValue(v);
                        float radius = i/* * mUnit * 0.001*/;
                        radius *= cos(piching);

                        float x_0 = imageS * 0.5 + sin(heading) * radius;
                        float x_1 = imageS * 0.5 + sin(heading + stepRadian) * radius;
                        float y_0 = imageT * 0.5 + cos(heading) * radius;
                        float y_1 = imageT * 0.5 + cos(heading + stepRadian) * radius;

                        osg::Vec2 dir = (osg::Vec2(x_1, y_1) - osg::Vec2(x_0, y_0));
                        float length = dir.length();
                        dir.normalize();

                        //sample acr
                        for (int l = 0; l < length; l++)
                        {
                            float x = x_0 + dir.x() * l;
                            float y = y_0 + dir.y() * l;
                            int imageCol_1 = int(floor(x));
                            int imageRow_1 = int(floor(y));
                            int imageCol_2 = int(ceil(x));
                            int imageRow_2 = int(ceil(y));
                            _SetImageValue(image, imageCol_1, imageRow_1, color);
                            *(float*)floatImage->data(imageCol_1, imageRow_1, 0) = v;
                            //sample nearest
                            if (imageCol_1 != imageCol_2 || imageRow_1 != imageRow_2)
                            {
                                if (*(float*)floatImage->data(imageCol_1, imageRow_2, 0) < v)
                                {
                                    *(float*)floatImage->data(imageCol_1, imageRow_2, 0) = v;
                                    _SetImageValue(image, imageCol_1, imageRow_2, color);
                                }
                                if (*(float*)floatImage->data(imageCol_2, imageRow_1, 0) < v)
                                {
                                    *(float*)floatImage->data(imageCol_2, imageRow_1, 0) = v;
                                    _SetImageValue(image, imageCol_2, imageRow_1, color);
                                }
                                if (*(float*)floatImage->data(imageCol_2, imageRow_2, 0) < v)
                                {
                                    *(float*)floatImage->data(imageCol_2, imageRow_2, 0) = v;
                                    _SetImageValue(image, imageCol_2, imageRow_2, color);
                                }
                            }
                        }
                    }
                 }

                mImageList.push_back(image);
                mFloatImageList.push_back(floatImage);
            }
        }
           
        bool CinRadReaderFMT::convert(std::istream& sstream)
        {
            parseData(sstream);
            generateImage();
            osg::ref_ptr<osg::Image> image = generateVolumeImage();
            if (image.valid())
            {
                setUint8Image(image);
            }

            return true;
        }
    }
}

