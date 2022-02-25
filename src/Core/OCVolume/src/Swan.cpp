#include "Swan.h"


namespace OC
{
    namespace Volume
    {
		struct swanheader
		{
			char ZonName[12];
			char DataName[38];
			char	Flag[8];
			char	Version[8];
			unsigned short int year;//2008 两个字节
			unsigned short int month;//05  两个字节
			unsigned short int day;//19    两个字节
			unsigned short int hour;//14   两个字节
			unsigned short int minute;//31 两个字节
			unsigned short int interval;  //两个字节        
			unsigned short int XNumGrids;//1300 两个字节
			unsigned short int YNumGrids;//800 两个字节
			unsigned short int ZNumGrids;//20  两个字节
			int RadarCount; //拼图雷达数 四个字节
			float StartLon; //网格开始经度（左上角） 四个字节
			float StartLat; //网格开始纬度（左上角） 四个字节
			float CenterLon;//网格中心经度 四个字节
			float CenterLat;//网格中心纬度 四个字节
			float XReso;	//经度方向分辨率 四个字节
			float YReso;	//纬度方向分辨率 四个字节
			float ZhighGrids[40];//垂直方向的高度（单位km）数目根据ZnumGrids而得（最大40层） 160个字节。
			char RadarStationName[20][16];    //雷达站点名称,	20*16字节
			float  RadarLongitude[20];      //雷达站点所在经度，单位：度， 4*20字节
			float  RadarLatitude[20];       //雷达站点所在纬度，单位：度， 4*20字节
			float  RadarAltitude[20];      //雷达所在海拔高度，单位：米， 4*20字节
			unsigned char    MosaicFlag[20];    //该雷达数据是否包含在本次拼图中，未包含:0，包含:1, 20字节
			char	Reserved[172];			//备用
		};

        CSwan::CSwan()
        {
			mClampMin= 0.0;
			mClampMax = 70.0;
        }

        CSwan::~CSwan()
        {

        }

		inline float functionDecodeDBZ (uint8 dbz)
		{
			return (dbz - 66.0) / 2.0;
		}

        bool CSwan::convert(std::stringstream& sstream)
        {
			swanheader header;
			sstream.read((char*)&header, sizeof(swanheader));
			size_t size = (size_t)header.XNumGrids * (size_t)header.YNumGrids * (size_t)header.ZNumGrids;

			std::vector<uint8> buffer;
			buffer.resize(size);

			sstream.read((char*)&buffer[0], size);

			int imageS = header.XNumGrids;
			int imageT = header.YNumGrids;
			int imageR = header.ZNumGrids;
			//calc param
			float w_degree = (header.CenterLon - header.StartLon) * 2.0;
			float h_degree = (header.StartLat - header.CenterLat) * 2.0;

			osg::Vec3d center(header.CenterLon, header.CenterLat, 0.0);
			float d_meters = (header.ZhighGrids[imageR - 1] - header.ZhighGrids[0]) * 1000.0;
			float w_meters = width_in_meter(header.StartLat - h_degree, w_degree);
			float h_meters = height_in_meter(h_degree);

			//
			osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);

			float scaler = mClampMax - mClampMin;

			long offset = 0;
			for (long z = 0; z < imageR; z++)
			{
				for (long y = 0; y < imageT; y++)
				{
					for (long x = 0; x < imageS; x++, offset++)
					{
						float v = functionDecodeDBZ(buffer[offset]);

						if (v < 1e-3)
						{
							continue;
						}

						PRC1(v);
						osg::Vec4ub color = _EncodeValue(v);
						unsigned char* data = (unsigned char*)image->data(x, imageT - 1 - y, z);
						data[0] = color.r();
						data[1] = color.g();
						data[2] = data[3] = 0;

						if (y > 0 && x > 0 && y < imageT - 1 && x < imageS - 1)
						{
							float px = functionDecodeDBZ(buffer[offset + 1]);
							float nx = functionDecodeDBZ(buffer[offset - 1]);
							float py = functionDecodeDBZ(buffer[offset + imageS]);
							float ny = functionDecodeDBZ(buffer[offset - imageS]);
	
							osg::Vec2 octVec = computeNormal(v, px, nx, px, ny);
							data[2] = octVec.x();
							data[3] = octVec.y();
						}
					}
				}
			}

			setPosition(center);
			setMeter(osg::Vec3d(w_meters, h_meters, d_meters));
			setUint8Image(image);
			return true;
        }

    }
}