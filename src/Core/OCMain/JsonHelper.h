#ifndef _OC_JSONHELPER_H__
#define _OC_JSONHELPER_H__

#include "JsonPackage.h"
#include "Common.h"

namespace OC
{
	class _MainExport JsonHelper
	{
	public:
		JsonHelper(void);
		~JsonHelper(void);

		static std::vector<std::pair<String,String> > parseStringPairList(String jsonStr);
		static NameValuePairList parseNameValuePairList(String jsonStr);
		static StringMap parseStyleString(String styleStr);
		static osg::MixinVector<osg::Vec3d> parseGeometryJson(String jsonStr);
	};
	
}


#endif



