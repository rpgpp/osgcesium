#ifndef _OC_VOLUME_OBJECT_H__
#define _OC_VOLUME_OBJECT_H__

#include "VolumeDefine.h"
#include <osg/Image>
#include <osg/Vec4>
#include <osg/Vec4ub>
#include <osg/ValueObject>
#include "OCUtility/StringConverter.h"
#include "OCUtility/StringUtil.h"
#include "OCUtility/FileUtil.h"
#include "OCUtility/RGBAEncoder.h"

namespace OC
{
    namespace Volume
    {
        class _VoloumeExport CVolumeObject : public osg::Image
        {
        public:
            CVolumeObject();
            virtual ~CVolumeObject();
            virtual bool convert(std::string filename) { return false; }
            virtual bool convert(std::istream& sstream) { return false; }

            osg::Vec4ub _EncodeValue(float value);

            osg::Image* uint8Image();
            void setUint8Image(osg::Image* image);
            osg::Image* floatImage();
            void setExtent(CRectangle rect);
            void setExtent(double xmin, double ymin, double xmax, double ymax);
            void setInternalFormat(int internalformat);
            void setMeter(float width,float height,float depth);
            void setMeter(osg::Vec3d whd);
            void setPosition(osg::Vec3d position);
            void setPosition(CVector2 position);
            void setClampMinMax(float clampmin, float clampmax);
            String getMeta();
            void writeToW3DM(std::string filename);
            void writeToStream(std::ostream& ofs);
            void writeToZipfile(std::string filename);
            String getVarName() { return mVarname; }
            void setVarName(String name) { mVarname = name; }
            
            void setStatistic(String name,double value) 
            { 
                mStatistics.push_back(std::make_pair(name, value));
                mStatisticMatrix.push_back(value);
            };
            static osg::Vec2 computeNormal(float value,float positiveX, float nagativeX, float positiveY, float nagativeY);
            static osg::Image* _CreateVolumeImage(int s, int t, int r, GLenum pixelFormat = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
        protected:
            int                                mPixelFormat;
            float                              mClampMin;
            float                              mClampMax;
            String                             mVarname;
            doubleVector                       mStatisticMatrix;
            typedef std::pair<String, double> StatisticPair;
            typedef std::vector<StatisticPair > StatisticVector;
            StatisticVector             mStatistics;
            osg::Vec4d                  mExtent;
            osg::Vec3d                  mMeter;
            osg::Vec3d                  mPosition;
            osg::ref_ptr<osg::Image>    mUint8Image;
            osg::ref_ptr<osg::Image>    mFloatImage;
        };


#define EARTH_RADIUS 6378137.f
#define PRC1(v) v = (int(v*10))*0.1
#define PRC2(v) v = (int(v*100))*0.01
        void _SetImageValue(osg::Image* image, int col, int row, osg::Vec4ub color); 

        inline float height_in_degree(float height_in_meter)
        {
            return height_in_meter * 180.0 / (EARTH_RADIUS * osg::PI);
        }

        inline float width_in_degree(float lat_in_degree, float width_in_meter)
        {
            return width_in_meter * 180.0 / (cos(osg::DegreesToRadians(lat_in_degree)) * EARTH_RADIUS * osg::PI);
        }

        inline float width_in_meter(float lat_in_degree, float width_in_degree)
        {
            return cos(osg::DegreesToRadians(lat_in_degree)) * EARTH_RADIUS * osg::PI * (width_in_degree / 180.0);
        }

        inline float height_in_meter(float height_in_degree)
        {
            return EARTH_RADIUS * osg::PI * (height_in_degree / 180.0);
        }
    }
}



#endif // !_OC_VOLUME_OBJECT_H__



