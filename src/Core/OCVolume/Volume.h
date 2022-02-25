#ifndef _OCVOLUME_H__
#define _OCVOLUME_H__

#include "VolumeObject.h"

namespace OC
{
    namespace Volume
    {
        class _VoloumeExport CVolume
        {
        public:
            CVolume();
            ~CVolume();

            static CVolumeObject* convert(String filename, String varname = StringUtil::BLANK);
        };
    }
}

#endif // !_OCVOLUME_H__
