#ifndef _OC_FyAwx_H__
#define _OC_FyAwx_H__

#include "VolumeObject.h"

namespace OC
{
	class FyAwx : public Volume::CVolumeObject
	{
	public:
		FyAwx() {}
		~FyAwx() {}
		virtual bool convert(std::string filename);
	};
}

#endif // !_OC_FyAwx_H__



