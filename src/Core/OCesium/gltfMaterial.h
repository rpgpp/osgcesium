#ifndef _GLTF_MATERAILS_H__
#define _GLTF_MATERAILS_H__

#include "gltfConfig.h"

namespace OC
{
    namespace Cesium
    {
        class _CesiumExport gltfMaterial : public osg::Object
        {
        public:
            gltfMaterial(){}
            gltfMaterial(osg::StateSet* stateSet);
            gltfMaterial(const gltfMaterial& eh, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
            META_Object(OCesium, gltfMaterial);
            String key();

            int materialsId = -1;
            osg::Texture* texture = NULL;
            osg::ref_ptr<osg::Image> image;
            int min_filter = 0;
            int max_filter = 0;
            int wrap_s = 0;
            int wrap_t = 0;
            bool doubleSide = false;
            bool blend = false;
            GLenum mode = osg::PrimitiveSet::TRIANGLES;
            double roughnessFactor = 1.0;
            double metallicFactor = 1.0;
            osg::Vec4 diffuse = osg::Vec4(1.0, 1.0, 1.0, 1.0);
            osg::Vec4 ambient = osg::Vec4(0.2, 0.2, 0.2, 1.0);
            osg::Vec4 specular = osg::Vec4(0.0, 0.0, 0.0, 1.0);
            osg::ref_ptr<osg::UByteArray> imageData;
            bool KHR_materials_unlit = false;
            osg::Vec2 offset = osg::Vec2(0.0,0.0);
            osg::Vec2 scale = osg::Vec2(1.0,1.0);
            bool operator == (const gltfMaterial& rkMat) const;
            bool operator < (const gltfMaterial& rkMat) const;
        };
    }
}


#endif