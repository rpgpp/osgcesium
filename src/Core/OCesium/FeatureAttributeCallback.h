#ifndef _FEATURE_ATTRIBUTE_CALLBACK_H__
#define _FEATURE_ATTRIBUTE_CALLBACK_H__

#include "gltfConfig.h"

namespace OC
{
    namespace Cesium
    {
        class _CesiumExport FeatureAttributeCallback : public osg::Referenced
        {
        public:
            FeatureAttributeCallback(){}
            virtual ~FeatureAttributeCallback() {}
            virtual void onSetAttirubte(String name,FeatureBatchTable* table){}
        };
    }
}

#endif