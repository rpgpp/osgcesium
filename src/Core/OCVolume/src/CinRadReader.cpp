#include "CinRadReader.h"
#include "OCMain/ConfigManager.h"

namespace OC
{namespace Volume{

	CinRadReader::CinRadReader(String name)
	{
		mRadius = 460.0;
		mClampMin = 0.0;
		mClampMax = 70.0;
		mUnit = 1000.0;
		parseName(name);
	}

	CinRadReader::~CinRadReader()
	{

	}

	void CinRadReader::parseName(String name)
	{
		StringVector names = StringUtil::split(osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(name)), "_");
		StringVector::iterator it = names.begin();
		for (; it != names.end(); it++)
		{
			String str = *it;
			if (StringUtil::startsWith(str, "Z9", false))
			{
				CVector2 pos = Singleton(ConfigManager).getVector2Value(str);
				if (pos != CVector2::ZERO)
				{
					setPosition(pos);
				}
			}
			else if (str == "CA")
			{
				mRadius = 920.0;
			}
		}
	}

	void CinRadReader::parseData(std::istream& sstream)
	{
		CINRAD_RecordCommon data;

		if (mRadius == 460.0)
		{
			data.bufferDBZ.resize(460);
			data.bufferDoppler.resize(920);
			data.bufferSpectralWidth.resize(920);
		}
		else
		{
			data.bufferDBZ.resize(800);
			data.bufferDoppler.resize(1600);
			data.bufferSpectralWidth.resize(1600);
		}

		size_t struSize = sizeof(CINRAD_Record) +
			data.bufferDBZ.size() +
			data.bufferDoppler.size() +
			data.bufferSpectralWidth.size() +
			4;

		while (sstream.read((char*)&data.header, sizeof(CINRAD_Record)))
		{
			if (!sstream.read((char*)&(data.bufferDBZ[0]), data.bufferDBZ.size()))
				break;
			if (!sstream.read((char*)&(data.bufferDoppler[0]), data.bufferDoppler.size()))
				break;
			if (!sstream.read((char*)&(data.bufferSpectralWidth[0]), data.bufferSpectralWidth.size()))
				break;
			sstream.clear();
			sstream.seekg(4, std::ios::cur);
			mPitchRadialMapDBZ[data.header.PitchingNo].push_back(data);
		}
	}

	inline float decodeDBZ(unsigned char code)
	{
		float v = (code == 1 || code == 0) ? 0.0 : (code - 2) / 2.0 - 32.0;
		PRC1(v);
		return v;
	}

	void CinRadReader::generateImage()
	{
		int imageS = 2 * mRadius;
		int imageT = imageS;

		std::vector<osg::ref_ptr<osg::Image> > imageList;
		for (TListMap::iterator it = mPitchRadialMapDBZ.begin();
			it != mPitchRadialMapDBZ.end();it++)
		{
			TList& list = it->second;
			float piching = list[0].header.Pitching / 8.0 * (180.0 / 4096.0);

			long validCount = 0;
			osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, 1);
			osg::ref_ptr<osg::Image> floatImage = _CreateVolumeImage(imageS, imageT, 1);
			for (TList::iterator record = list.begin();
				record != list.end(); record++)
			{
				float heading = record->header.Heading / 8.0 * (180.0 / 4096.0);
				for (int i = 0; i < record->header.NumStockOfDBZ; i++)
				{
					float v = decodeDBZ(record->bufferDBZ[i]);

					if (v <= 0.0)
					{
						continue;
					}

					validCount++;

					osg::Vec4ub color = _EncodeValue(v);

					float radius = (record->header.DistanceOfFirstDBZ + i * record->header.PerStockLengthOfDBZ);
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

			if (validCount > 0)
			{
				mPitchList.push_back(piching);
				imageList.push_back(image);
				mFloatImageList.push_back(floatImage);
			}
		}

	}

	bool CinRadReader::convert(std::istream& sstream)
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