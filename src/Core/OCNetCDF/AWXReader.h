#pragma once

#include "NetCDFDefine.h"
#include "OCVolume/VolumeObject.h"

namespace OC
{
	class AWXReader : public Volume::CVolumeObject
	{
	public:
		AWXReader() {}
		~AWXReader() {}
		virtual bool convert(std::string filename);

		osg::Image* readImage(String filename, String var_name);
	};
}

