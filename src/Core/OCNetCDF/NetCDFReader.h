#ifndef _OC_NETCDFREADER_H_
#define _OC_NETCDFREADER_H_

#include "NetCDFDefine.h"
#include "OCVolume/VolumeObject.h"

namespace OC
{
	class _NetCDFExport CNetCDFReader
	{
	public:
		CNetCDFReader();
		~CNetCDFReader();
		
		osg::Image* readNpy(String filename, String var);
		osg::Image* readAWX(String filename, String var);
		osg::Image* readGRB2(String filename, String var);
		osg::Image* readNC(String filename, String var);
		osg::Image* readSW(String filename, String var);
		Volume::CVolumeObject* readVolume(String filename, String var);
	};
}


#endif // !1

