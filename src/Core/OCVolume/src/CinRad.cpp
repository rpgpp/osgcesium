#include "CinRad.h"

using namespace OC;
using namespace OC::Volume;

inline osg::Vec3d computeDestVec(osg::Vec3d radiusDir, float radius, osg::Vec3d center, float pitch)
{
	osg::Vec3d localY = radiusDir ^ osg::Z_AXIS;
	osg::Quat quat;
	quat.makeRotate(pitch, localY);
	return quat * radiusDir * radius + center;
}

osg::Image* CinRad::generateVolumeImage()
{
	if (mFloatImageList.size() < 1)
	{
		return NULL;
	}

	int imageS = mFloatImageList[0]->s();
	int imageT = mFloatImageList[0]->t();
	int imageR = 21;

	float w_meters = imageS * mUnit;
	float h_meters = imageS * mUnit;
	float d_meters = imageR * 1000.0;
	setMeter(osg::Vec3d(w_meters, h_meters, d_meters));

	int interval = 1;
	if (imageS > 920)
	{
		interval = imageS / 920;
		imageS = imageT = 920;
	}

	osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);

	int imageRadius = imageS * 0.5;
	osg::Vec3 center(imageRadius, imageRadius, 0.0);

	for (int i = 0; i < (int)mPitchList.size(); i++)
	{
		osg::Image* srcImage = mFloatImageList[i].get();
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

				osg::Vec3d destVector = computeDestVec(radiusDir, radius, center, pitch);
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
								osg::Vec3d destVector2 = computeDestVec(radiusDir, radius, center, osg::DegreesToRadians(mPitchList[i + 1]));
								osg::Vec3d dir = destVector2 - destVector;
								float len = dir.length();
								dir.normalize();
								for (int k = 1; k < len; k++)
								{
									osg::Vec3d destVector3 = destVector + dir * k;
									int x = osg::clampBetween((int)destVector3.x(), 0, imageS - 1);
									int y = osg::clampBetween((int)destVector3.y(), 0, imageT - 1);
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

	osg::ref_ptr<osg::Image> image2 = _CreateVolumeImage(imageS, imageT, imageR);
	// interop
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

				osg::Vec4ub color = _EncodeValue(v);

				unsigned char* data2 = image2->data(s, t, r);
				data2[0] = color.r();
				data2[1] = color.g();
				data2[2] = data2[3] = 0;

				if (s > 0 && t > 0 && s < imageS - 1 && t < imageT - 1)
				{
					float px = *(float*)image->data(s + 1, t, r);
					float nx = *(float*)image->data(s - 1, t, r);
					float py = *(float*)image->data(s, t + 1, r);
					float ny = *(float*)image->data(s, t -1, r);
					osg::Vec2 octVec = computeNormal(v, px, nx, py, ny);
					data2[2] = octVec.x();
					data2[3] = octVec.y();
				}
			}
		}
	}

	return image2.release();
}