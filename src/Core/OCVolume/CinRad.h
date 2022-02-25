#ifndef _OC_CinRad_H__
#define _OC_CinRad_H__

#include "VolumeObject.h"
#include "CinRadHeader.h"

namespace OC
{
	namespace Volume
	{
		class CinRad : public CVolumeObject
		{
		public:
			CinRad() {}
			virtual ~CinRad() {}
			virtual osg::Image* generateVolumeImage();

		protected:
			typedef std::vector<osg::ref_ptr<osg::Image> > ImageList;
			int                     mRadius;
			float					mUnit;
			ImageList               mImageList;
			ImageList				mFloatImageList;
			std::vector<float>		mPitchList;
		};
	}
}

#endif // !_OC_CinRad_H__

