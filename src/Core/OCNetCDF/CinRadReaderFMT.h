#pragma once

#include "ICinRadReader.h"

namespace OC
{
	template<class T>
	class CinRadReaderFMT : public ICinRadReader
	{
		typedef std::vector<T> TList;
		typedef std::map<int,TList > TListMap;
	public:
		CinRadReaderFMT(Type type) :ICinRadReader(type) {}
		~CinRadReaderFMT() {}

		void read(DataStreamPtr dataStream)
		{
			if (dataStream->size() < sizeof(CINRAD_RecordFMT))
			{
				return;
			}

			size_t readSize = 0;
			CINRAD_RecordFMT record;
			readSize = dataStream->read((char*)&record, sizeof(CINRAD_RecordFMT));
			if (readSize != sizeof(CINRAD_RecordFMT))
			{
				return;
			}

			int cutNumber = record.TaskConfig.cutNumber;
			if (cutNumber < 1 || cutNumber > 33)
			{
				return;
			}

			mPosition.x = record.SiteConfig.longitude;
			mPosition.y = record.SiteConfig.latitude;

			std::vector<CINRAD_RecordFMT_CutConfig> headerConfigs;
			headerConfigs.resize(cutNumber);
			for (int i = 0; i < cutNumber; i++)
			{
				dataStream->read((char*)&headerConfigs[i], sizeof(CINRAD_RecordFMT_CutConfig));
			}

			mUnit = headerConfigs[0].logResolution;
			mDoupplerUnit = headerConfigs[0].dopplerResolution;

			T headerData;
			readSize = dataStream->read((char*)&headerData, sizeof(CINRAD_RadialDataFMT));
			while (readSize == sizeof(CINRAD_RadialDataFMT))
			{
				for (int i = 0; i < headerData.Radial.momentNumber; i++)
				{
					size_t rs = dataStream->read((char*)&headerData.Moment, sizeof(CINRAD_MomentDataFMT));
					if (rs != sizeof(CINRAD_MomentDataFMT))
					{
						pitchRadialMapDBZ.clear();
						return;
					}
					int elevNo = headerData.Radial.elevationNumber;
					if (headerData.Moment.dataType == 2)
					{
						//DBZ
						if (headerData.Moment.length > _radius)
						{
							_radius = headerData.Moment.length;
						}
						headerData.bufferDBZ.resize(headerData.Moment.length);
						rs = dataStream->read((char*)&(headerData.bufferDBZ[0]), headerData.Moment.length);
						if (rs != headerData.Moment.length)
						{
							pitchRadialMapDBZ.clear();
							return;
						}
						pitchRadialMapDBZ[elevNo].push_back(headerData);
					}
					else if (headerData.Moment.dataType == 3)
					{
						//Doppler Velocity
						if (headerData.Moment.length > _radiusDoupplerV)
						{
							_radiusDoupplerV = headerData.Moment.length;
						}
						headerData.bufferDoppler.resize(headerData.Moment.length);
						rs = dataStream->read((char*)&(headerData.bufferDoppler[0]), headerData.Moment.length);
						if (rs != headerData.Moment.length)
						{
							return;
						}
						pitchRadialMapV[elevNo].push_back(headerData);
					}
					else if (headerData.Moment.dataType == 4)
					{
						//Spectrum Width
						headerData.bufferSpectralWidth.resize(headerData.Moment.length);
						rs = dataStream->read((char*)&(headerData.bufferSpectralWidth[0]), headerData.Moment.length);
						if (rs != headerData.Moment.length)
						{
							return;
						}
						pitchRadialMapW[elevNo].push_back(headerData);
					}
					else
					{
						dataStream->skip(headerData.Moment.length);
					}
				}

				readSize = dataStream->read((char*)&headerData, sizeof(CINRAD_RadialDataFMT));
			}
		}

		inline float decodeDBZ(T t,int i)
		{
			uint8 code = t.bufferDBZ[i];
			int scale = t.Moment.scale;
			int offset = t.Moment.offset;
			float v = (code < 5) ? 0.0 : (code - offset) / scale;
			PRC1(v);
			return v;
		}

		virtual ImageList generateImage(float clampMin, float clampMax,bool autoResize = true)
		{
			ImageList imageList;

			float scaler = clampMax - clampMin;

			int imageS = s();
			int imageT = t();

			auto it = pitchRadialMapDBZ.begin();

			static float stepRadian = osg::DegreesToRadians(0.9836);

			for (;it!= pitchRadialMapDBZ.end(); it++)
			{
				TList& list = it->second;

				float piching = list[0].Radial.elevation;
				pitchList.push_back(piching);
				piching = osg::DegreesToRadians(piching);

				osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, 1);
				osg::ref_ptr<osg::Image> floatImage = _CreateVolumeImage(imageS, imageT, 1, GL_RED, GL_FLOAT);

				for (auto& record : list)
				{
					float heading = record.Radial.azimunth;
					heading = osg::DegreesToRadians(heading);
					for (int i = 0; i < record.Moment.length; i++)
					{
						float v = decodeDBZ(record,i);

						if (v <= 0.0)
						{
							continue;
						}

						osg::Vec4ub color = _EncodeValue(v, clampMin, scaler);
						
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
							int imageCol_1 = Math::IFloor(x);
							int imageRow_1 = Math::IFloor(y);
							int imageCol_2 = Math::ICeil(x);
							int imageRow_2 = Math::ICeil(y);
							_SetImageValue(image,imageCol_1, imageRow_1, color);
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

				if (image->s() > 920 && autoResize)
				{
					image->scaleImage(920, 920, image->r());
					floatImage->scaleImage(920, 920, floatImage->r());
				}
				imageList.push_back(image);
				mFloatImageList.push_back(floatImage);
			}

			return imageList;
		}

		float						mUnit = 1000.0;
		float						mDoupplerUnit = 250.0;
		CVector3					mPosition = CVector3::ZERO;
		TListMap					pitchRadialMapDBZ;
		TListMap					pitchRadialMapV;
		TListMap					pitchRadialMapW;
		osg::ref_ptr<osg::Image>	mVelocityImage;
	};
}

