#include "CINRad.h"
#include "CinRadReaderFMT.h"
#include "CinRadReader.h"

namespace OC
{
	OC::CINRad::CINRad(String filename)
		: mFilename(filename)
		, mType(UNKNOWN)
		, mRadarRadius(460.0)
	{
		mClampMax = 70.0;
		parseName(filename);
	}

	OC::CINRad::~CINRad()
	{
	}

	void CINRad::parseName(String filename)
	{
		StringVector names = StringUtil::split(osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(filename)), "_");
		for (auto str : names)
		{
			if (StringUtil::startsWith(str, "Z9", false))
			{
				CVector2 pos = Singleton(ConfigManager).getVector2Value(str);
				if(pos != CVector2::ZERO)
					CVolume::setPosition(pos);
			}
			else if (str == "SA" || str == "SB")
			{
				mType = SASB;
			}
			else if (str == "CA")
			{
				mType = CA;
			}
			else if (str == "FMT")
			{
				mType = SASBCAXFMT;
			}
		}
	}

	ImageList CINRad::readImageList(String filename, String var_name, bool usefloat)
	{
		ImageList imageList;
		if (UNKNOWN == mType)
		{
			return imageList;
		}

		//mDataStream = CVolume::openbz2(filename);

		if (!mDataStream.isNull())
		{
			if (mType == CA)
			{
				auto reader = CinRadReader<CINRAD_RecordCA>(CA);
				reader.read(mDataStream);
				mRadarRadius = reader.s();
				imageList = reader.generateImage(mClampMin, mClampMax);
				if (usefloat)
				{
					imageList = reader.mFloatImageList;
				}
				mPitchList = reader.pitchList;
			}
			else if (mType == SASB)
			{
				auto reader = CinRadReader<CINRAD_RecordSASB>(SASB);
				reader.read(mDataStream);
				imageList = reader.generateImage(mClampMin, mClampMax);
				if (usefloat)
				{
					imageList = reader.mFloatImageList;
				}
				mPitchList = reader.pitchList;
				mRadarRadius = reader.s();
			}
			else if (mType == SASBCAXFMT)
			{
				auto CinRadReader = CinRadReaderFMT<CINRAD_DataFMT>(SASBCAXFMT);
				CinRadReader.read(mDataStream);
				imageList = CinRadReader.generateImage(mClampMin, mClampMax, !usefloat);
				if (usefloat)
				{
					imageList = CinRadReader.mFloatImageList;
				}
				//FMT extra infomation
				setPosition(CinRadReader.mPosition);
				mPitchList = CinRadReader.pitchList;
				mUnit = CinRadReader.mUnit;
				mVelocityImage = CinRadReader.mVelocityImage;
				mRadarRadius = CinRadReader.s();
			}
		}

		return imageList;
	}

	osg::Image* OC::CINRad::readImage(String filename, String var_name)
	{
		ImageList imageList = readImageList(filename,var_name);

		if (imageList.size() == 0)
		{
			std::cout << filename << ": unhandle size" << imageList.size() << std::endl;
			return NULL;
		}

		if (imageList.size() != 9)
		{
			std::cout << filename << ": bad size!!!" << imageList.size() << std::endl;
		}

		int imageS = imageList[0]->s();
		int imageT = imageList[0]->t();
		int imageR = (int)imageList.size();

		osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);
		
		for (int i = 0; i < imageR; i++)
		{
			osg::Image* srcImage = imageList[i].get();
			unsigned char* dest = image->data(0, 0, i);
			unsigned char* src = srcImage->data(0, 0, 0);
			memcpy(dest, src, srcImage->getTotalSizeInBytes());
#if 0
			String dir = osgDB::getFilePath(filename);
			dir += "/CIN_RAD/";
			osgDB::makeDirectory(dir); 
			String simplefilename = osgDB::getSimpleFileName(osgDB::getNameLessAllExtensions(filename)) + "_" + StringConverter::toString(i) + ".png";
			String imageFile = dir + simplefilename;
			osgDB::writeImageFile(*srcImage, imageFile);
#endif
		}

		
		float w_meters = mRadarRadius * mUnit;
		float h_meters = mRadarRadius * mUnit;
		float d_meters = 20.0 * mUnit;

		setMeter(w_meters, h_meters, d_meters);
		CVolume::setVolumeAttribute(image);
		return image.release();
	}

	inline float decode(unsigned char* b,float scale)
	{
		osg::Vec4 color(b[0] / 255.0, b[1] / 255.0, b[2] / 255.0, b[3] / 255.0);
		return RGBAEncoder::decodeRGBAFloat(color) * scale;
	}

	inline bool greaterThan(unsigned char* lghs, unsigned char* rghs)
	{
		return *(float*)lghs > * (float*)rghs;
	}

	osg::Image* CINRad::readImageForVolume(String filename, String var_name)
	{
		ImageList imageList = readImageList(filename, var_name, true);

		if (imageList.size() < 1)
		{
			std::cout << filename << ": bad size" << imageList.size() << std::endl;
			return NULL;
		}

		float scale = mClampMax - mClampMin;

		int imageS = imageList[0]->s();
		int imageT = imageList[0]->t();
		int imageR = 20;
		osg::ref_ptr<osg::Image> image;

		int interval = 1;
		if (imageS > 920)
		{
			interval = imageS / 920;
			imageS = imageT = 920;
		}
		if (0)
		{
			imageR = mPitchList.size();
			image = _CreateVolumeImage(imageS, imageT, imageR);

			int imageRadius = imageS * 0.5;
			CVector3 center(imageRadius, imageRadius, 0.0);

			for (int r = 0; r < (int)mPitchList.size(); r++)
			{
				osg::Image* srcImage = imageList[r].get();

				for (int s = 0; s < imageS; s++)
				{
					for (int t = 0; t < imageT; t++)
					{
						for (int s1 = s * interval; s1 < interval * (s + 1); s1++)
						{
							for (int t1 = t * interval; t1 < interval * (t + 1); t1++)
							{
								unsigned char* data = srcImage->data(s1, t1, 0);
								unsigned char* dest = image->data(s, t, r);
								float v = *(float*)data;
								if (v < 1e-3)
								{
									continue;
								}

								osg::Vec4ub color = _EncodeValue(v, mClampMin, scale);
								dest[0] = color.r();
								dest[1] = color.g();
								dest[2] = color.b();
								dest[3] = color.a();
							}
						}
					}
				}
			}
		}
		else
		{
			imageR = 21;
			image = _CreateVolumeImage(imageS, imageT, imageR);

			int imageRadius = imageS * 0.5;
			osg::Vec3d center(imageRadius, imageRadius, 0.0);

			for (int i = 0; i < (int)mPitchList.size(); i++)
			{
				osg::Image* srcImage = imageList[i].get();
				float pitch = osg::DegreesToRadians(mPitchList[i]);
				for (int s = 0; s < imageS; s++)
				{
					for (int t = 0; t < imageT; t++)
					{
						osg::Vec3d radiusDir = osg::Vec3d(s, t, 0.0) - center;
						float radius = radiusDir.length();
						if (radius > imageRadius)
						{
							continue;
						}

						radiusDir.normalize();
						auto computeDestVec = [radiusDir, radius, center](float pitch)
						{
							osg::Vec3d localY = radiusDir ^ osg::Z_AXIS;
							osg::Quat quat;
							quat.makeRotate(pitch, localY);
							return quat * radiusDir * radius + center;
						};

						osg::Vec3d destVector = computeDestVec(pitch);
						int x = destVector.x();
						int y = destVector.y();
						float height = destVector.z();
						int r = height;
						if (r > imageR - 1)
							continue;

						float* dest = (float*)image->data(x, y, r);

						//
						for (int s1 = s * interval; s1 < interval * (s + 1); s1++)
						{
							for (int t1 = t * interval; t1 < interval * (t + 1); t1++)
							{
								{
 									float value = *(float*)srcImage->data(s1, t1, 0);
									if (value < 1e-3)
									{
										continue;
									}

									if (*dest < value)
									{
										*dest = value;
									}

									if (i < mPitchList.size() - 1)
									{
										osg::Vec3d destVector2 = computeDestVec(osg::DegreesToRadians(mPitchList[i + 1]));
										osg::Vec3d dir = destVector2 - destVector;
										float len = dir.length();
										dir.normalize();
										for (int k = 1; k < len; k++)
										{
											osg::Vec3d destVector3 = destVector + dir * k;
											int x = Math::Clamp((int)destVector3.x(), 0, imageS - 1);
											int y = Math::Clamp((int)destVector3.y(), 0, imageT - 1);
											float height = destVector3.z();
											int r = height;
											if (r > imageR - 1)
												continue;
											float* dest = (float*)image->data(x, y, r);
											if (*dest < value)
											{
												*dest = value;
											}
										}
									}
								}

							}
						}
						//
					}
				}
			}

			for (int r = 0; r < imageR; r++)
			{
				for (int s = 0; s < imageS; s++)
				{
					for (int t = 0; t < imageT; t++)
					{
						unsigned char* data = image->data(s, t, r);
						float v = *(float*)data;
						if (v < 1e-3)
						{
							continue;
						}

						osg::Vec4ub color = _EncodeValue(v, mClampMin, scale);
						data[0] = color.r();
						data[1] = color.g();
						data[2] = data[3] = 0;

						if (s < imageS - 1 && t < imageT - 1)
						{
							float px = *(float*)image->data(s + 1, t, r);
							float py = *(float*)image->data(s, t + 1, r);
							//float pz = *(float*)image->data(s, t, r + 1);

							float dx = px - v;
							float dy = py - v;
							//float dz = pz - v;
							if (dx != 0.0 || dy != 0.0 /*|| dz != 0.0*/)
							{
								osg::Vec3 normal = osg::Vec3(dx, dy, min(dx, dy));
								normal.normalize();
								osg::Vec2 octVec = RGBAEncoder::octEncode(normal);
								data[2] = octVec.x();
								data[3] = octVec.y();
							}
						}
					}
				}
			}
		}

		float w_meters = mRadarRadius * mUnit;
		float h_meters = mRadarRadius * mUnit;
		float d_meters = imageR * 1000.0;

		setMeter(w_meters, h_meters, d_meters);
		CVolume::setVolumeAttribute(image);
		return image.release();
	}
}
