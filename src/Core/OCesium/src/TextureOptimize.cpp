#include "TextureOptimize.h"
#include "TextureAtlasBuilder.h"

namespace OC
{namespace Cesium
{
    void TextureOptimize::merge_clamp(BatchGeodeList& mBatchGeodeList, BatchGeodeMap& mBatchGeodeMap)
    {
        BatchGeodeList batchGeodeList;
        osg::ref_ptr<TextureAtlasBuilder> _builderBlend = new TextureAtlasBuilder;
        osg::ref_ptr<TextureAtlasBuilder> _builderOpacity = new TextureAtlasBuilder;

        int atlasSize = Cesium::gltfConfig::AtlasImageSize;
        if (atlasSize != 0)
        {
            _builderBlend->setMaximumAtlasSize(atlasSize, atlasSize);
            _builderOpacity->setMaximumAtlasSize(atlasSize, atlasSize);
        }
        _builderBlend->setMargin(0);
        _builderOpacity->setMargin(0);

        for (auto i : mBatchGeodeList)
        {
            BatchGeode* batchGeode = i.get();
            gltfMaterial* material = batchGeode->material();
            if (material->image.valid()
                && ((material->wrap_s == osg::Texture::WrapMode::CLAMP_TO_EDGE
                    && material->wrap_t == osg::Texture::WrapMode::CLAMP_TO_EDGE)
                    || !batchGeode->mWrapSTOutofRange))
            {
                if (material->blend)
                {
                    _builderBlend->addSource(material->image);
                }
                else
                {
                    _builderOpacity->addSource(material->image);
                }
            }
        }
        _builderBlend->buildAtlas();
        _builderOpacity->buildAtlas();

        for (auto i : mBatchGeodeList)
        {
            BatchGeode* batchGeode = i.get();
            gltfMaterial* material = batchGeode->material();

            osg::Image* img = NULL;
            if (material->blend)
            {
                img = _builderBlend->getImageAtlas(material->image);
                if (img)
                {
                    batchGeode->transTexCoord(_builderBlend->getTextureMatrix(material->image));
                    material->image = img;
                    String key = material->key();
                    if (!mBatchGeodeMap[key].valid())
                    {
                        mBatchGeodeMap[key] = batchGeode;
                        batchGeodeList.push_back(batchGeode);
                    }
                    else
                    {
                        mBatchGeodeMap[key]->merge(batchGeode);
                    }
                }
                else
                {
                    if (find(batchGeodeList.begin(), batchGeodeList.end(), batchGeode) == batchGeodeList.end())
                        batchGeodeList.push_back(batchGeode);
                }
            }
            else
            {
                img = _builderOpacity->getImageAtlas(material->image);
                if (img)
                {
                    batchGeode->transTexCoord(_builderOpacity->getTextureMatrix(material->image));
                    material->image = img;
                    String key = material->key();
                    if (!mBatchGeodeMap[key].valid())
                    {
                        mBatchGeodeMap[key] = batchGeode;
                        batchGeodeList.push_back(batchGeode);
                    }
                    else
                    {
                        mBatchGeodeMap[key]->merge(batchGeode);
                    }
                }
                else
                {
                    if (find(batchGeodeList.begin(), batchGeodeList.end(), batchGeode) == batchGeodeList.end())
                        batchGeodeList.push_back(batchGeode);
                }
            }
        }

        mBatchGeodeList.clear();
        mBatchGeodeList.swap(batchGeodeList);
        std::cout << ",Batch Clamp:(" << mBatchGeodeList.size() << ")";
    }

    void TextureOptimize::merge_repeat(BatchGeodeList& mBatchGeodeList, BatchGeodeMap& mBatchGeodeMap)
    {
        if (mBatchGeodeList.size() < 2)
        {
            return;
        }

        BatchGeodeList batchGeodeList;

        osg::ref_ptr<TextureAtlasBuilder> _builderBlend = new TextureAtlasBuilder;
        osg::ref_ptr<TextureAtlasBuilder> _builderOpacity = new TextureAtlasBuilder;
        int atlasSize = Cesium::gltfConfig::AtlasImageSize;
        if (atlasSize != 0)
        {
            _builderBlend->setMaximumAtlasSize(atlasSize, atlasSize);
            _builderOpacity->setMaximumAtlasSize(atlasSize, atlasSize);
        }
        _builderBlend->setMargin(0);
        _builderOpacity->setMargin(0);

        for (auto i : mBatchGeodeList)
        {
            BatchGeode* batchGeode = i.get();
            gltfMaterial* material = batchGeode->material();

            //if (!batchGeode->mHasErrorTexCoord && material->image.valid()
            //    && material->wrap_s == osg::Texture::WrapMode::REPEAT
            //    && material->wrap_t == osg::Texture::WrapMode::REPEAT
            //    && batchGeode->mWrapSTOutofRange)
            if (material->image.valid())
            {
                if (material->blend)
                {
                    _builderBlend->addSource(material->image);
                }
                else
                {
                    _builderOpacity->addSource(material->image);
                }
            }
            else
            {
                if (find(batchGeodeList.begin(), batchGeodeList.end(), batchGeode) == batchGeodeList.end())
                    batchGeodeList.push_back(batchGeode);
            }
        }
        _builderBlend->buildAtlas();
        _builderOpacity->buildAtlas();

        for (auto i : mBatchGeodeList)
        {
            BatchGeode* batchGeode = i.get();
            gltfMaterial* material = batchGeode->material();

            osg::Image* img = NULL;
            if (material->blend)
            {
                img = _builderBlend->getImageAtlas(material->image);
                if (img)
                {
                    material->offset = _builderBlend->getTextureOffset(material->image);
                    material->scale = _builderBlend->getTextureScale(material->image);
                    material->image = img;
                    batchGeode->setTextureTransform(material->offset, material->scale);
                    String key = material->key();
                    if (!mBatchGeodeMap[key].valid())
                    {
                        mBatchGeodeMap[key] = batchGeode;
                        batchGeodeList.push_back(batchGeode);
                    }
                    else
                    {
                        mBatchGeodeMap[key]->merge(batchGeode);
                    }
                }
            }
            else
            {
                img = _builderOpacity->getImageAtlas(material->image);
                if (img)
                {
                    material->offset = _builderOpacity->getTextureOffset(material->image);
                    material->scale = _builderOpacity->getTextureScale(material->image);
                    material->image = img;
                    batchGeode->setTextureTransform(material->offset, material->scale);
                    String key = material->key();
                    if (!mBatchGeodeMap[key].valid())
                    {
                        mBatchGeodeMap[key] = batchGeode;
                        batchGeodeList.push_back(batchGeode);
                    }
                    else
                    {
                        mBatchGeodeMap[key]->merge(batchGeode);
                    }
                }
                else
                {
                    if (find(batchGeodeList.begin(), batchGeodeList.end(), batchGeode) == batchGeodeList.end())
                        batchGeodeList.push_back(batchGeode);
                }
            }
        }

        mBatchGeodeList.clear();
        mBatchGeodeList.swap(batchGeodeList);
        std::cout << ",Batch repeat:(" << mBatchGeodeList.size() << ")";
    }

    void TextureOptimize::scaleDimansion(osg::Image* image)
    {
        if ((image->getPixelFormat() == GL_RGB || image->getPixelFormat() == GL_RGBA))
        {
            int maxSize = Cesium::gltfConfig::MaxTextureSize;
            if (maxSize == 0)
            {
                return;
            }

            if ((image->s() > maxSize || image->t() > maxSize))
            {
                if (image->t() > image->s())
                {
                    int t = maxSize;
                    int s = maxSize * 1.0 * image->s() / image->t();
                    image->scaleImage(s, t, 1);
                }
                else
                {
                    int s = maxSize;
                    int t = maxSize * 1.0 * image->t() / image->s();
                    image->scaleImage(s, t, 1);
                }
            }
        }
    }
}
}