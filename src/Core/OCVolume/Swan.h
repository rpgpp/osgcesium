#ifndef _OC_VOLUME_SWAN_H__
#define _OC_VOLUME_SWAN_H__

#include "VolumeObject.h"

namespace OC
{
    namespace Volume
    {
        class CSwan : public CVolumeObject
        {
        public:
            CSwan();
            ~CSwan();
            bool convert(std::stringstream& sstream);
        };
    }
}

#endif // !_OC_VOLUME_SWAN_H__




