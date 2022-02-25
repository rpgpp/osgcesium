#include "gltfMaterial.h"
#include "OCMain/osgearth.h"

namespace OC
{
    namespace Cesium
    {
        gltfMaterial::gltfMaterial(osg::StateSet* stateSet)
        {
            if (stateSet == NULL)
            {
                roughnessFactor = gltfConfig::roughnessFactor;
                metallicFactor = gltfConfig::metallicFactor;
            }
            else
            {
                if (texture = dynamic_cast<osg::Texture*>(stateSet->getTextureAttribute(0, osg::StateAttribute::TEXTURE)))
                {
                    image = texture->getImage(0);
                    min_filter = texture->getFilter(osg::Texture::MIN_FILTER);
                    max_filter = texture->getFilter(osg::Texture::MAG_FILTER);
                    wrap_s = texture->getWrap(osg::Texture::WRAP_S);
                    wrap_t = texture->getWrap(osg::Texture::WRAP_T);

                    if (wrap_s == osg::Texture::CLAMP || wrap_s == osg::Texture::CLAMP_TO_BORDER)
                    {
                        wrap_s = osg::Texture::CLAMP_TO_EDGE;
                    }
                    if (wrap_t == osg::Texture::CLAMP || wrap_t == osg::Texture::CLAMP_TO_BORDER)
                    {
                        wrap_t = osg::Texture::CLAMP_TO_EDGE;
                    }
                }

                if (osg::Material* materials = dynamic_cast<osg::Material*>(stateSet->getAttribute(osg::StateAttribute::MATERIAL, 0)))
                {
                    diffuse = materials->getDiffuse(osg::Material::FRONT);
                    ambient = materials->getAmbient(osg::Material::FRONT);
                    specular = materials->getSpecular(osg::Material::FRONT);
                    
                    osg::Material::ColorMode colorMode = materials->getColorMode();
                    if (colorMode == osg::Material::ColorMode::SPECULAR)
                    {
                        diffuse = (diffuse + specular) * 0.5;
                    }
                    else if (colorMode == osg::Material::ColorMode::AMBIENT)
                    {
                        diffuse = (diffuse + ambient) * 0.5;
                    }

                    //if (image && image->isImageTranslucent())
                    //{
                    //    stateSet->getAttribute
                    //    diffuse.w() = 0.5;
                    //}
                    
                    roughnessFactor = gltfConfig::roughnessFactor;
                    metallicFactor = gltfConfig::metallicFactor;

                    blend = diffuse.w() < 1.0;
                }

                if (!blend && osg::StateAttribute::ON == stateSet->getMode(GL_BLEND))
                {
                    if (image.valid())
                    {
                        blend = image->isImageTranslucent();
                    }
                }
            }

            if (Cesium::gltfConfig::disableLight)
            {
                diffuse = osg::Vec4(1.0, 1.0, 1.0, 1.0);
                roughnessFactor = 1.0;
                metallicFactor = 1.0;
                KHR_materials_unlit = true;
            }
        }

        String gltfMaterial::key()
        {
            return osgEarth::Stringify() 
                << "minfilter:" << min_filter << "|"
                << "maxfilter:" << max_filter << "|"
                << "wraps:" << wrap_s << "|"
                << "wrapt:" << wrap_t << "|"
                << "image:" << image.get() << "|"
                << "mode:" << mode << "|"
                << "blend:" << blend << "|"
                << "roughness:" << int(roughnessFactor) << "|"
                << "metallic:" << int(metallicFactor) << "|"
                << "diffuse.r:" << int(diffuse.x() * 100) << "|"
                << "diffuse.g:" << int(diffuse.y() * 100) << "|"
                << "diffuse.b:" << int(diffuse.z() * 100) << "|"
                << "diffuse.a:" << int(diffuse.w() * 100)
                ;
        }

        bool gltfMaterial::operator == (const gltfMaterial& rkMat) const
        {
        
            return (min_filter == rkMat.min_filter
                && max_filter == rkMat.max_filter
                && wrap_s == rkMat.wrap_s
                && wrap_t == rkMat.wrap_t
                && image.get() == rkMat.image.get()
                && mode == rkMat.mode
                && blend == rkMat.blend
                && osg::equivalent(diffuse.x(), rkMat.diffuse.x(), 1e-1f)
                && osg::equivalent(diffuse.y(), rkMat.diffuse.y(), 1e-1f)
                && osg::equivalent(diffuse.z(), rkMat.diffuse.z(), 1e-1f)
                && osg::equivalent(diffuse.w(), rkMat.diffuse.w(), 1e-1f)
                && osg::equivalent(roughnessFactor, rkMat.roughnessFactor, 1e-1)
                && osg::equivalent(metallicFactor, rkMat.metallicFactor, 1e-1)
                );
        }

        bool gltfMaterial::operator < (const gltfMaterial& rkMat) const
        {
            return (min_filter < rkMat.min_filter
                || max_filter < rkMat.max_filter
                || wrap_s < rkMat.wrap_s
                || wrap_t < rkMat.wrap_t
                || image.get() < rkMat.image.get()
                || mode < rkMat.mode
                || blend < rkMat.blend
                || (int(diffuse.x() * 100) < int(rkMat.diffuse.x() * 100)
                    || int(diffuse.y() * 100) < int(rkMat.diffuse.y() * 100)
                    || int(diffuse.w() * 100) < int(rkMat.diffuse.w() * 100))
                || (int(roughnessFactor * 100) < int(rkMat.roughnessFactor * 100))
                || (int(metallicFactor * 100) < int(rkMat.metallicFactor * 100))
                );
        }

        /*
        
                bool gltfMaterial::operator == (const gltfMaterial& rkMat) const
        {
        
            return (min_filter == rkMat.min_filter
                && max_filter == rkMat.max_filter
                && wrap_s == rkMat.wrap_s
                && wrap_t == rkMat.wrap_t
                && image == rkMat.image
                && mode == rkMat.mode
                && osg::equivalent(diffuse.x(), rkMat.diffuse.x(), 1e-1f)
                && osg::equivalent(diffuse.y(), rkMat.diffuse.y(), 1e-1f)
                && osg::equivalent(diffuse.z(), rkMat.diffuse.z(), 1e-1f)
                && osg::equivalent(diffuse.w(), rkMat.diffuse.w(), 1e-1f)
                && osg::equivalent(roughnessFactor, rkMat.roughnessFactor, 1e-1)
                && osg::equivalent(metallicFactor, rkMat.metallicFactor, 1e-1)
                );
        }

        bool gltfMaterial::operator < (const gltfMaterial& rkMat) const
        {
            return (min_filter < rkMat.min_filter
                || max_filter < rkMat.max_filter
                || wrap_s < rkMat.wrap_s
                || wrap_t < rkMat.wrap_t
                || image < rkMat.image
                || mode < rkMat.mode
                || (diffuse.x() - rkMat.diffuse.x()) < 1e-1f
                || (diffuse.y() - rkMat.diffuse.y()) < 1e-1f
                || (diffuse.z() - rkMat.diffuse.z()) < 1e-1f
                || (diffuse.w() - rkMat.diffuse.w()) < 1e-1f
                || (roughnessFactor - rkMat.roughnessFactor) < 1e-1
                || (metallicFactor - rkMat.metallicFactor) < 1e-1
                );
        }
        */
    }
}
