#ifndef __BuildWell_H__
#define __BuildWell_H__
#include "OCModel/ModelDefine.h"

namespace OC
{
	namespace Modeling
	{
		class BuildWell
		{
		public:
			BuildWell(void)
			{
			}

			BuildWell(const double br,const double height,const String wt,const String ct)
			{
				setRadius(br,br /* * 0.7*/);
				setHeight(height*3/8,height*3/8,height/4);
				setTexturePath(wt,ct);
			}
			void setRadius(const double br,const double tr)
			{
				mBottomRadius = br;
				mTopRadius = tr;
			}
			void setHeight(const double bh,const double mh,const double th)
			{
				mBottomHeight = bh;
				mMiddleHeight = mh;
				mTopHeight = th;
			}
			void setTexturePath(const String wt,const String ct)
			{
				mWellWallImage = wt;
				mWellCoverImage = ct;
			}		
			osg::ref_ptr<osg::MatrixTransform> build();

			void setImageInternal(bool inter)
			{
				mImageInternal = inter;
			}
		private:	
			bool						mImageInternal;
			double						mBottomRadius;
			double						mTopRadius;
			double						mBottomHeight;	
			double						mMiddleHeight;
			double						mTopHeight;
			String						mWellWallImage;
			String						mWellCoverImage;

			Vector3List setCircle(CVector3 center,CVector3 normal,double dr,int loop,Vector2List & texturecoor,double TextureYvalue);
			osg::ref_ptr<osg::Group>	 createWellHatCylinder();
			osg::ref_ptr<osg::Geometry > createWellHat();
		};
	}
}
#endif
