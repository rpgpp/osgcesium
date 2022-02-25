#pragma once

#include "ICinRadReader.h"

namespace OC
{
    class CINRad : public CVolume
    {
	public:
		CINRad(String filename);
		~CINRad();

		osg::Image* readImage(String filename, String var_name);
		osg::Image* readImageForVolume(String filename, String var_name);
		void setType(Type type){mType = type;}
	private:
		ImageList readImageList(String filename, String var_name,bool usefloat = false);
		void parseName(String filename);
		int							mRadarRadius;
		float						mUnit = 1000.0f;
		Type						mType;
		String						mFilename;
		DataStreamPtr				mDataStream;
		std::vector<float>			mPitchList;
		osg::ref_ptr<osg::Image>	mVelocityImage;
	};
}

