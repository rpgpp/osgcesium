#ifndef _KTX_COMPRESS_H__
#define _KTX_COMPRESS_H__

#include "CesiumDefine.h"

namespace OC
{
    namespace Cesium
    {
        class _CesiumExport KtxCompress
        {
        public:
            KtxCompress();
            ~KtxCompress();
            
            static osg::UByteArray* writeImage(osg::Image* image);
        };
    }
}//namespace

#endif