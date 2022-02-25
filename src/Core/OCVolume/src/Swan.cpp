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
			unsigned short int year;//2008 �����ֽ�
			unsigned short int month;//05  �����ֽ�
			unsigned short int day;//19    �����ֽ�
			unsigned short int hour;//14   �����ֽ�
			unsigned short int minute;//31 �����ֽ�
			unsigned short int interval;  //�����ֽ�        
			unsigned short int XNumGrids;//1300 �����ֽ�
			unsigned short int YNumGrids;//800 �����ֽ�
			unsigned short int ZNumGrids;//20  �����ֽ�
			int RadarCount; //ƴͼ�״��� �ĸ��ֽ�
			float StartLon; //����ʼ���ȣ����Ͻǣ� �ĸ��ֽ�
			float StartLat; //����ʼγ�ȣ����Ͻǣ� �ĸ��ֽ�
			float CenterLon;//�������ľ��� �ĸ��ֽ�
			float CenterLat;//��������γ�� �ĸ��ֽ�
			float XReso;	//���ȷ���ֱ��� �ĸ��ֽ�
			float YReso;	//γ�ȷ���ֱ��� �ĸ��ֽ�
			float ZhighGrids[40];//��ֱ����ĸ߶ȣ���λkm����Ŀ����ZnumGrids���ã����40�㣩 160���ֽڡ�
			char RadarStationName[20][16];    //�״�վ������,	20*16�ֽ�
			float  RadarLongitude[20];      //�״�վ�����ھ��ȣ���λ���ȣ� 4*20�ֽ�
			float  RadarLatitude[20];       //�״�վ������γ�ȣ���λ���ȣ� 4*20�ֽ�
			float  RadarAltitude[20];      //�״����ں��θ߶ȣ���λ���ף� 4*20�ֽ�
			unsigned char    MosaicFlag[20];    //���״������Ƿ�����ڱ���ƴͼ�У�δ����:0������:1, 20�ֽ�
			char	Reserved[172];			//����
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