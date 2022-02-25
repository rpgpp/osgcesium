#pragma once

#include "ICinRadReader.h"

namespace OC
{
	template<class T>
	class CinRadReader : public ICinRadReader
	{
		typedef std::vector<T> TList;
	public:
		CinRadReader(Type type) :ICinRadReader(type) {}
		~CinRadReader() {}

		void read(DataStreamPtr dataStream)
		{
			size_t nLengthSize = dataStream->size();
			int struSize = (int)sizeof(T);
			if (nLengthSize > struSize)
			{
				T record;
				int readSize = dataStream->read((char*)&record, struSize);
				while (readSize == struSize)
				{
					pitchRadialMapDBZ[record.header.PitchingNo].push_back(record);
					readSize = dataStream->read((char*)&record, struSize);
				}
			}
		}

		inline float decodeDBZ(unsigned char code)
		{
			float v = (code == 1 || code == 0) ? 0.0 : (code - 2) / 2.0 - 32.0;
			PRC1(v);
			return v;
		}

		ImageList generateImage(float clampMin, float clampMax, bool autoResize = true)
		{
			float scaler = clampMax - clampMin;
			int imageS = s();
			int imageT = t();

			std::vector<osg::ref_ptr<osg::Image> > imageList;
			for (auto& it : pitchRadialMapDBZ)
			{
				TList& list = it.second;
				float piching = list[0].header.Pitching / 8.0 * (180.0 / 4096.0);

				long validCount = 0;
				osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, 1);
				osg::ref_ptr<osg::Image> floatImage = _CreateVolumeImage(imageS, imageT, 1);
				for (auto& record : list)
				{
					float heading = record.header.Heading / 8.0 * (180.0 / 4096.0);
					for (int i = 0; i < record.header.NumStockOfDBZ; i++)
					{
						float v = decodeDBZ(record.bufferDBZ[i]);

						if (v <= 0.0)
						{
							continue;
						}

						validCount++;

						osg::Vec4ub color = _EncodeValue(v, clampMin, scaler);

						float radius = (record.header.DistanceOfFirstDBZ + i * record.header.PerStockLengthOfDBZ);
						radius *= cos(osg::DegreesToRadians(piching));
						radius *= 0.001;
						float x_0 = imageS * 0.5 + sin(osg::DegreesToRadians(heading)) * radius;
						float x_1 = imageS * 0.5 + sin(osg::DegreesToRadians(heading + 0.9836)) * radius;
						float y_0 = imageT * 0.5 + cos(osg::DegreesToRadians(heading)) * radius;
						float y_1 = imageT * 0.5 + cos(osg::DegreesToRadians(heading + 0.9836)) * radius;

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

				if (validCount > 0)
				{
					pitchList.push_back(piching);
					imageList.push_back(image);
					mFloatImageList.push_back(floatImage);
				}
			}

			return imageList;
		}

		std::map<int, TList >	pitchRadialMapDBZ;
	};
}
