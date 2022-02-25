#ifndef _WEBP_ENCODER_H__
#define _WEBP_ENCODER_H__

#include "CesiumDefine.h"

namespace OC
{
    namespace Cesium
    {
        class _CesiumExport WebpEncoder
        {
        public:
            WebpEncoder();
            ~WebpEncoder();

            static osgDB::ReaderWriter::WriteResult writeImage(const osg::Image& img, std::ostream& fout);
        };
    }
}

#endif

