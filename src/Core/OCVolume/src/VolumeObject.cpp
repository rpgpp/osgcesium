#include "VolumeObject.h"
#include "OCZip/PakoZip.h"
#include "OCZip/zip.h"
#include "OCMain/json/value.h"
#include "OCMain/json/writer.h"
#include "OCMain/DataStream.h"

namespace OC
{namespace Volume{

    CVolumeObject::CVolumeObject()
    {
        mPixelFormat = GL_RGBA;
            mClampMin = 0.0;
            mClampMax = 100.0;
    }

    CVolumeObject::~CVolumeObject()
    {

    }

    void _SetImageValue(osg::Image* image, int col, int row, osg::Vec4ub color)
    {
        unsigned char* data = (unsigned char*)image->data(col, row, 0);
        memcpy(data, &color[0], 4);
    }

    osg::Vec2 CVolumeObject::computeNormal(float value, float positiveX, float nagativeX, float positiveY, float nagativeY)
    {
        osg::Vec2 nor(0.0,0.0);
        float dx = (std::min)(positiveX, nagativeX) - value;
        float dy = (std::min)(positiveY, nagativeY) - value;
        if (dx != 0.0 || dy != 0.0)
        {
            osg::Vec3 normal = osg::Vec3(dx, dy, (std::max)(dx, dy));
            normal.normalize();
            nor = RGBAEncoder::octEncode(normal);
        }
        return nor;
    }

    osg::Image* CVolumeObject::_CreateVolumeImage(int s, int t, int r, GLenum pixelFormat, GLenum type)
    {
        osg::ref_ptr<osg::Image> image = new osg::Image;
        image->allocateImage(s, t, r, pixelFormat, type);
        memset(image->data(0, 0, 0), 0, image->getTotalSizeInBytes());
        return image.release();
    }

    osg::Vec4ub CVolumeObject::_EncodeValue(float value)
    {
        osg::Vec4 color = RGBAEncoder::encodeFloatRGBA(Clamp((value - mClampMin) / (mClampMax - mClampMin), 0.0f, 0.999999f));
        osg::Vec4ub colorUb;
        colorUb.r() = (color.r() * 255.0);
        colorUb.g() = (color.g() * 255.0);
        colorUb.b() = (color.b() * 255.0);
        colorUb.a() = (color.a() * 255.0);
        return colorUb;
    }

    osg::Image* CVolumeObject::uint8Image()
    {
        return mUint8Image.get();
    }

    void CVolumeObject::setUint8Image(osg::Image* image)
    {
        mUint8Image = image;
    }

    osg::Image* CVolumeObject::floatImage()
    {   
        return mFloatImage.get();
    }

    void CVolumeObject::setExtent(CRectangle rect)
    {
        mExtent.x() = rect.getMinimum().x;
        mExtent.y() = rect.getMinimum().y;
        mExtent.z() = rect.getMaximum().x;
        mExtent.w() = rect.getMaximum().y;
    }

    void CVolumeObject::setExtent(double xmin, double ymin, double xmax, double ymax)
    {
        mExtent.x() = xmin;
        mExtent.y() = ymin;
        mExtent.z() = xmax;
        mExtent.w() = ymax;
    }

    void CVolumeObject::setInternalFormat(int internalformat)
    {
        mPixelFormat = internalformat;
    }

    void CVolumeObject::setMeter(float width, float height, float depth)
    {
        mMeter.x() = width;
        mMeter.y() = height;
        mMeter.z() = depth;
    }
    
    void CVolumeObject::setMeter(osg::Vec3d whd)
    {
        mMeter = whd;
    }

    void CVolumeObject::setPosition(CVector2 position)
    {
        mPosition.x() = position.x;
        mPosition.y() = position.y;
        mPosition.z() = 0.0;
    }

    void CVolumeObject::setPosition(osg::Vec3d position)
    {
        mPosition = position;
    }

    void CVolumeObject::setClampMinMax(float clampmin, float clampmax)
    {
        mClampMin = clampmin;
        mClampMax = clampmax;
    }

    String CVolumeObject::getMeta()
    {
        Json::Value root(Json::objectValue);
        root["position"]["x"] = mPosition.x();
        root["position"]["y"] = mPosition.y();
        root["position"]["z"] = mPosition.z();
        root["source"]["width"] = mUint8Image->s();
        root["source"]["height"] = mUint8Image->t();
        root["source"]["depth"] = mUint8Image->r();
        root["meter"]["width"] = mMeter.x();
        root["meter"]["height"] = mMeter.y();
        root["meter"]["depth"] = mMeter.z();
        root["u_s_o"]["scaler"] = mClampMax - mClampMin;
        root["u_s_o"]["clampMin"] = mClampMin;
        root["u_s_o"]["clampMax"] = mClampMax;
        root["extent"]["xmin"] = mExtent.x();
        root["extent"]["ymin"] = mExtent.y();
        root["extent"]["xmax"] = mExtent.z();
        root["extent"]["ymax"] = mExtent.w();
        root["internalFormat"] = mPixelFormat;
        root["pixelFormat"] = mPixelFormat;

        //for (StatisticVector::iterator it = mStatistics.begin(); it != mStatistics.end(); it++)
        //{
        //    String name = it->first;
        //    double value = it->second;
        //}

        Json::Value statisticValue(Json::arrayValue);
        for (doubleVector::iterator it = mStatisticMatrix.begin(); it != mStatisticMatrix.end(); it++)
        {
            double value = *it;
            statisticValue.append(value);
        }

        root["statistic"] = statisticValue;

        Json::FastWriter writer;

        String mFeatureJSON = writer.write(root);
        return mFeatureJSON;
    }

    void CVolumeObject::writeToStream(std::ostream& os)
    {
        String mFeatureJSON = getMeta();

        w3dmheader header;
        header.magic[0] = 'w', header.magic[1] = '3', header.magic[2] = 'd', header.magic[3] = 'm';
        header.version = 1;

        //json
        header.featureTableJSONByteLength = mFeatureJSON.length();
        os.write((const char*)&header, sizeof(header));
        os.write(mFeatureJSON.c_str(), header.featureTableJSONByteLength);
        if (os.fail())
        {
            return;
        }

        osg::Image& image = *mUint8Image;

        std::stringstream sstream;
        size_t writeBeforeSize = os.tellp();

        // write image data
        int ps = osg::Image::computeNumComponents(image.getPixelFormat());
        for (int r = 0; r < image.r(); ++r)
        {
            for (int t = 0; t < image.t(); ++t)
            {
                const char* data = (const char*)image.data(0, t, r);
                sstream.write((const char*)data, (std::streamsize)image.s() * ps);
            }
        }

        bool ret = PakoZip::compress(os, sstream.str());

        if (!ret)
        {
            return;
        }

        header.featureTableBinaryByteLength = os.tellp();
        header.featureTableBinaryByteLength -= writeBeforeSize;
        header.byteLength =
            sizeof(w3dmheader) +
            header.featureTableJSONByteLength +
            header.featureTableBinaryByteLength;

        os.seekp(std::ios::beg);
        os.write((const char*)&header, sizeof(header));
        os.seekp(std::ios::end);
    }

    void CVolumeObject::writeToW3DM(std::string filename)
    {
        std::ofstream ofs(filename.c_str(), std::ios::binary);
        if (ofs)
        {
            writeToStream(ofs);
            ofs.close();
        }
    }

    void CVolumeObject::writeToZipfile(std::string filename)
    {
        if (!mUint8Image.valid())
        {
            return;
        }
        String mFeatureJSON = getMeta();
        osg::Image& image = *mUint8Image;
        size_t byteLen = image.getTotalSizeInBytes();
        MemoryDataStream dataStream(new char[byteLen], byteLen);
        int ps = osg::Image::computeNumComponents(image.getPixelFormat());
        for (int r = 0; r < image.r(); ++r)
        {
            for (int t = 0; t < image.t(); ++t)
            {
                const char* data = (const char*)image.data(0, t, r);
                dataStream.write((const char*)data, (std::size_t)image.s() * ps);
            }
        }
        if (zipFile f = zipOpen(filename.c_str(), APPEND_STATUS_CREATE))
        {
            String name, ext, path;
            StringUtil::splitFullFilename(filename, name, ext, path);
            if (ZIP_OK == zipOpenNewFileInZip(f, (name + ".raw").c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
            {
                if (ZIP_OK == zipWriteInFileInZip(f, dataStream.getPtr(), byteLen))
                {
                    zipCloseFileInZip(f);
                }
            }
            if (ZIP_OK == zipOpenNewFileInZip(f, (name + ".json").c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
            {
                if (ZIP_OK == zipWriteInFileInZip(f, mFeatureJSON.c_str(), mFeatureJSON.length()))
                {
                    zipCloseFileInZip(f);
                }
            }
            zipClose(f, 0);
        }
    }

}}