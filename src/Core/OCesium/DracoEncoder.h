#ifndef _DRACO_ENCODER_H__
#define _DRACO_ENCODER_H__

#include "CesiumDefine.h"

namespace OC
{
    namespace Cesium
    {
        class DracoEncoder
        {
        public:
            DracoEncoder();
            ~DracoEncoder();

            bool encode(BatchGeode* batchGeode);

            DracoAttributeMap mAttributeMap;
            const char* data();
            size_t size();
        private:
            draco::EncoderBuffer* buffer;
        };
    }
}

#endif

