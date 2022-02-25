#ifndef _VOLUMEMANAGER_H__
#define _VOLUMEMANAGER_H__

#include "OCServerDefine.h"
#include "OCMain/DataStream.h"
#include "OCMain/osg.h"

namespace OC
{
    namespace Server
    {
        class VolumeManager
        {
        public:
            static String getVolumeFile(const String name, const String varname, const String ext, const String dir, const String version = "1.0");

        };
    }
}

#endif


