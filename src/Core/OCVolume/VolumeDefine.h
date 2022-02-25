#ifndef _OC_VOLUME_DEF_H__
#define _OC_VOLUME_DEF_H__

#ifdef _WIN32
#ifndef OCVolume_EXPORTS
#define _VoloumeExport  __declspec(dllimport)
#else
#define _VoloumeExport  __declspec(dllexport)
#endif
#else
#define _VoloumeExport
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <vector>

namespace OC
{
    namespace Volume
    {
        struct w3dmheader
        {
            char magic[4];
            unsigned int version;
            unsigned int byteLength;
            unsigned int featureTableJSONByteLength;
            unsigned int featureTableBinaryByteLength;
        };

        class CVolume;
        class CVolumeObject;
        class CinRad;

        typedef unsigned char uint8;

        template <typename T>
        static T Clamp(T val, T minval, T maxval)
        {
            return (std::max)((std::min)(val, maxval), minval);
        }

    }
}

#endif // !_OC_VOLUME_H__
