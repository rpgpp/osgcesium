#include "FyAwx.h"
#include "OCMain/DataStream.h"

namespace OC
{
    struct Header1
    {
        char Sat96[12];
        short byteOrder;
        short sizeHeader1;
        short sizeHeader2;
        short sizeFilled;
        short sizeRecord;
        short numHeader;
        short numData;
        short typeProduct;
        short compressMode;
        char fmtFlag[8];
        short qualityFlag;
    };

    struct Header2
    {
        char satName[8];
        short year;
        short month;
        short day;
        short hour;
        short minute;
        short channel;
        short projection;
        short width;
        short height;
        short leftUpLine;
        short leftUpPixel;
        short sample;
        short boundN;//
        short boundS;
        short boundW;//����
        short boundE;
        short centerLat;
        short centerLon;
        short standard1;
        short standard2;
        short resolutionH;
        short resolutionV;
        short gridFlag;
        short gridvalue;
        short sizePalette;
        short sizeCalibration;
        short sizeNavigation;
        short reserved;
    };

    bool FyAwx::convert(std::string filename)
    {
        ifstream ifs(filename.c_str(), std::ios::binary);

        if (ifs)
        {
            size_t size1 = sizeof(Header1);
            size_t size2 = sizeof(Header2);

            Header1 header1;
            Header2 header2;
            ifs.read((char*)&header1, size1);
            ifs.read((char*)&header2, size2);

            ifs.clear();
            ifs.seekg(0, std::ios::beg);
            ifs.seekg(header1.sizeRecord * header1.numHeader);

            int dimX = header2.width;
            int dimY = header2.height;
            long size = dimX * dimY;

            mPixelFormat = GL_LUMINANCE;
            osg::ref_ptr<osg::Image> image = new osg::Image;
            image->allocateImage(dimX, dimY, 1, mPixelFormat, GL_UNSIGNED_BYTE);
            memset(image->data(0, 0, 0), 0, dimX * dimY * 1 * 1 * 1);

            MemoryDataStreamPtr dataStrea(new MemoryDataStream(size));
            size_t count = ifs.read((char*)dataStrea->getPtr(), size).gcount();

            if (count != size)
            {
                return false;
            }

            long i = 0;
            for (long y = 0; y < dimY; y++)
            {
                for (long x = 0; x < dimX; x++)
                {
                    unsigned char* data = (unsigned char*)image->data(x, dimY - y - 1, 0);
                    unsigned char v = dataStrea->getPtr()[i++];
                    osg::Vec4 color;
                    color.w() = v;
                    data[0] = color.w();
                }
            }

            ifs.close();

            float west = header2.boundW * 0.01;
            float east = header2.boundE * 0.01;
            float north = header2.boundN * 0.01;
            float south = header2.boundS * 0.01;

            {
                west = header2.centerLon - (header2.boundE - header2.boundW) * 0.5;
                east = header2.centerLon + (header2.boundE - header2.boundW) * 0.5;
                north = header2.centerLat + (header2.boundN - header2.boundS) * 0.5;
                south = header2.centerLat - (header2.boundN - header2.boundS) * 0.5;

                west *= 0.01;
                east *= 0.01;
                north *= 0.01;
                south *= 0.01;
            }


            CRectangle rect;
            rect.setMinimum(west, south);
            rect.setMaximum(east, north);


            CVector2 center = rect.getCenter();
            setExtent(rect);
            setUint8Image(image);
        }

        return true;
    }
}

