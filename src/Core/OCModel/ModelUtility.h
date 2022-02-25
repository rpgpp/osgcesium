#ifndef _MODEL_UTILITY_H_
#define _MODEL_UTILITY_H_

#include "PipeMath.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport ModelUtility
		{
		public:
			ModelUtility(void);
			~ModelUtility(void);
			static String fixshelftable(String table);
			static void getMinMaxSection(LinePtrList linkLines,PipeSection& minSect,PipeSection& maxSect);
			static Vector3List parseSpatialField(String geomStr);
			static Vector3List parseGeom(uchar* ptr);
			static PipeSection parseSect(String sectStr);
			static void setTexture(osg::Node* node,String texture,bool tInternal = false);
			static osg::StateSet* createTextureStateset(String imageurl,bool readimage);
		};
	}
}


#endif


