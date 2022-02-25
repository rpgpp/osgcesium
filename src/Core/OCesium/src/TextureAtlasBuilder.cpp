#include "TextureAtlasBuilder.h"

namespace OC
{
    namespace Cesium
    {

TextureAtlasBuilder::TextureAtlasBuilder() :
    _maximumAtlasWidth(2048),
    _maximumAtlasHeight(2048),
    _margin(8)
{
}

void TextureAtlasBuilder::reset()
{
    _sourceList.clear();
    _atlasList.clear();
}

void TextureAtlasBuilder::setMaximumAtlasSize(int width, int height)
{
    _maximumAtlasWidth = width;
    _maximumAtlasHeight = height;
}

void TextureAtlasBuilder::setMargin(int margin)
{
    _margin = margin;
}

void TextureAtlasBuilder::addSource(const osg::Image* image)
{
    if (!getSource(image)) _sourceList.push_back(new Source(image));
}

void TextureAtlasBuilder::addSource(const osg::Texture2D* texture)
{
    if (!getSource(texture)) _sourceList.push_back(new Source(texture));
}


void TextureAtlasBuilder::completeRow(unsigned int indexAtlas)
{
    AtlasList::iterator aitr = _atlasList.begin() + indexAtlas;
    //SourceList::iterator sitr = _sourceList.begin() + indexSource;
    Atlas* atlas = aitr->get();
    if (atlas->_indexFirstOfRow < atlas->_sourceList.size())
    {
        //Try to fill the row with smaller images.
        int x_max = atlas->_width - _margin;
        int y_max = atlas->_height - _margin;
        //int x_max = atlas->_maximumAtlasWidth  - _margin;
        //int y_max = atlas->_maximumAtlasHeight - _margin;

        // Fill last Row
        for (SourceList::iterator sitr3 = _sourceList.begin(); sitr3 != _sourceList.end(); ++sitr3)
        {
            int x_min = atlas->_x + _margin;
            int y_min = atlas->_y + _margin;
            if (y_min >= y_max || x_min >= x_max) continue;

            Source* source = sitr3->get();
            if (source->_atlas || atlas->_image->getPixelFormat() != source->_image->getPixelFormat() ||
                atlas->_image->getDataType() != source->_image->getDataType())
            {
                continue;
            }

            int image_s = source->_image->s();
            int image_t = source->_image->t();
            if (x_min + image_s <= x_max && y_min + image_t <= y_max)        // Test if the image can fit in the empty space.
            {
                source->_x = x_min;
                source->_y = y_min;
                //assert(source->_x + source->_image->s()+_margin <= atlas->_maximumAtlasWidth );        // "+_margin" and not "+2*_margin" because _x already takes the margin into account
                //assert(source->_y + source->_image->t()+_margin <= atlas->_maximumAtlasHeight);
                //assert(source->_x >= _margin);
                //assert(source->_y >= _margin);
                atlas->_x += image_s + 2 * _margin;
                //assert(atlas->_x <= atlas->_maximumAtlasWidth);
                source->_atlas = atlas;
                atlas->_sourceList.push_back(source);
            }
        }

        // Fill the last column
        SourceList srcListTmp;
        for (SourceList::iterator sitr4 = atlas->_sourceList.begin() + atlas->_indexFirstOfRow;
            sitr4 != atlas->_sourceList.end(); ++sitr4)
        {
            Source* srcAdded = sitr4->get();
            int y_min = srcAdded->_y + srcAdded->_image->t() + 2 * _margin;
            int x_min = srcAdded->_x;
            int x_max = x_min + srcAdded->_image->s();        // Hides upper block's x_max
            if (y_min >= y_max || x_min >= x_max) continue;

            Source* maxWidthSource = NULL;
            for (SourceList::iterator sitr2 = _sourceList.begin(); sitr2 != _sourceList.end(); ++sitr2)
            {
                Source* source = sitr2->get();
                if (source->_atlas || atlas->_image->getPixelFormat() != source->_image->getPixelFormat() ||
                    atlas->_image->getDataType() != source->_image->getDataType())
                {
                    continue;
                }
                int image_s = source->_image->s();
                int image_t = source->_image->t();
                if (x_min + image_s <= x_max && y_min + image_t <= y_max)        // Test if the image can fit in the empty space.
                {
                    if (maxWidthSource == NULL || maxWidthSource->_image->s() < source->_image->s())
                    {
                        maxWidthSource = source; //Keep the maximum width for source.
                    }
                }
            }
            if (maxWidthSource)
            {
                // Add the source with the max width to the atlas
                maxWidthSource->_x = x_min;
                maxWidthSource->_y = y_min;
                maxWidthSource->_atlas = atlas;
                srcListTmp.push_back(maxWidthSource); //Store the mawWidth source in the temporary vector.
            }
        }
        for (SourceList::iterator itTmp = srcListTmp.begin(); itTmp != srcListTmp.end(); ++itTmp)
        {
            //Add the sources to the general list (wasn't possible in the loop using the iterator on the same list)
            atlas->_sourceList.push_back(*itTmp);
        }
        atlas->_indexFirstOfRow = atlas->_sourceList.size();
    }
}

void TextureAtlasBuilder::buildAtlas()
{
    std::sort(_sourceList.begin(), _sourceList.end(), CompareSrc());        // Sort using the height of images
    _atlasList.clear();
    for (SourceList::iterator sitr = _sourceList.begin();
        sitr != _sourceList.end();
        ++sitr)
    {
        Source* source = sitr->get();
        if (!source->_atlas && source->suitableForAtlas(_maximumAtlasWidth, _maximumAtlasHeight, _margin))
        {
            bool addedSourceToAtlas = false;
            for (AtlasList::iterator aitr = _atlasList.begin();
                aitr != _atlasList.end() && !addedSourceToAtlas;
                ++aitr)
            {
                if (!(*aitr)->_image ||
                    ((*aitr)->_image->getPixelFormat() == (*sitr)->_image->getPixelFormat() &&
                        (*aitr)->_image->getPacking() == (*sitr)->_image->getPacking()))
                {
                    OSG_INFO << "checking source " << source->_image->getFileName() << " to see it it'll fit in atlas " << aitr->get() << std::endl;
                    TextureAtlasBuilder::Atlas::FitsIn fitsIn = (*aitr)->doesSourceFit(source);
                    if (fitsIn == TextureAtlasBuilder::Atlas::FITS_IN_CURRENT_ROW)
                    {
                        addedSourceToAtlas = true;
                        (*aitr)->addSource(source); // Add in the currentRow.
                    }
                    else if (fitsIn == TextureAtlasBuilder::Atlas::IN_NEXT_ROW)
                    {
                        completeRow(aitr - _atlasList.begin()); //Fill Empty spaces.
                        addedSourceToAtlas = true;
                        (*aitr)->addSource(source); // Add the source in the new row.
                    }
                    else
                    {
                        completeRow(aitr - _atlasList.begin()); //Fill Empty spaces before creating a new atlas.
                    }
                }
            }

            if (!addedSourceToAtlas)
            {
                OSG_INFO << "creating new Atlas for " << source->_image->getFileName() << std::endl;

                osg::ref_ptr<Atlas> atlas = new Atlas(_maximumAtlasWidth, _maximumAtlasHeight, _margin);
                _atlasList.push_back(atlas);
                if (!source->_atlas) atlas->addSource(source);
            }
        }
    }

    // build the atlas which are suitable for use, and discard the rest.
    AtlasList activeAtlasList;
    for (AtlasList::iterator aitr = _atlasList.begin();
        aitr != _atlasList.end();
        ++aitr)
    {
        osg::ref_ptr<Atlas> atlas = *aitr;

        if (atlas->_sourceList.size() == 1)
        {
            // no point building an atlas with only one entry
            // so disconnect the source.
            Source* source = atlas->_sourceList[0].get();
            source->_atlas = 0;
            atlas->_sourceList.clear();
        }

        if (!(atlas->_sourceList.empty()))
        {
            std::stringstream ostr;
            ostr << "atlas_" << activeAtlasList.size() << ".rgb";
            atlas->_image->setFileName(ostr.str());
            activeAtlasList.push_back(atlas);
            atlas->clampToNearestPowerOfTwoSize();
            atlas->copySources();
        }
    }
    // keep only the active atlas'
    _atlasList.swap(activeAtlasList);

}

osg::Image* TextureAtlasBuilder::getImageAtlas(unsigned int i)
{
    Source* source = _sourceList[i].get();
    Atlas* atlas = source ? source->_atlas : 0;
    return atlas ? atlas->_image.get() : 0;
}

osg::Texture2D* TextureAtlasBuilder::getTextureAtlas(unsigned int i)
{
    Source* source = _sourceList[i].get();
    Atlas* atlas = source ? source->_atlas : 0;
    return atlas ? atlas->_texture.get() : 0;
}

osg::Vec2 TextureAtlasBuilder::getTextureOffset(unsigned int i)
{
    Source* source = _sourceList[i].get();
    return source ? source->computeTextureOffset() : osg::Vec2();
}

osg::Vec2 TextureAtlasBuilder::getTextureScale(unsigned int i)
{
    Source* source = _sourceList[i].get();
    return source ? source->computeTextureScale() : osg::Vec2();
}

osg::Matrix TextureAtlasBuilder::getTextureMatrix(unsigned int i)
{
    Source* source = _sourceList[i].get();
    return source ? source->computeTextureMatrix() : osg::Matrix();
}

osg::Image* TextureAtlasBuilder::getImageAtlas(const osg::Image* image)
{
    Source* source = getSource(image);
    Atlas* atlas = source ? source->_atlas : 0;
    return atlas ? atlas->_image.get() : 0;
}

osg::Texture2D* TextureAtlasBuilder::getTextureAtlas(const osg::Image* image)
{
    Source* source = getSource(image);
    Atlas* atlas = source ? source->_atlas : 0;
    return atlas ? atlas->_texture.get() : 0;
}

osg::Vec2 TextureAtlasBuilder::getTextureOffset(const osg::Image* image)
{
    Source* source = getSource(image);
    return source ? source->computeTextureOffset() : osg::Vec2();
}

osg::Vec2 TextureAtlasBuilder::getTextureScale(const osg::Image* image)
{
    Source* source = getSource(image);
    return source ? source->computeTextureScale() : osg::Vec2();
}

osg::Matrix TextureAtlasBuilder::getTextureMatrix(const osg::Image* image)
{
    Source* source = getSource(image);
    return source ? source->computeTextureMatrix() : osg::Matrix();
}

osg::Image* TextureAtlasBuilder::getImageAtlas(const osg::Texture2D* texture)
{
    Source* source = getSource(texture);
    Atlas* atlas = source ? source->_atlas : 0;
    return atlas ? atlas->_image.get() : 0;
}

osg::Texture2D* TextureAtlasBuilder::getTextureAtlas(const osg::Texture2D* texture)
{
    Source* source = getSource(texture);
    Atlas* atlas = source ? source->_atlas : 0;
    return atlas ? atlas->_texture.get() : 0;
}

osg::Matrix TextureAtlasBuilder::getTextureMatrix(const osg::Texture2D* texture)
{
    Source* source = getSource(texture);
    return source ? source->computeTextureMatrix() : osg::Matrix();
}

TextureAtlasBuilder::Source* TextureAtlasBuilder::getSource(const osg::Image* image)
{
    for (SourceList::iterator itr = _sourceList.begin();
        itr != _sourceList.end();
        ++itr)
    {
        if ((*itr)->_image == image) return itr->get();
    }
    return 0;
}

TextureAtlasBuilder::Source* TextureAtlasBuilder::getSource(const osg::Texture2D* texture)
{
    for (SourceList::iterator itr = _sourceList.begin();
        itr != _sourceList.end();
        ++itr)
    {
        if ((*itr)->_texture == texture) return itr->get();
    }
    return 0;
}

bool TextureAtlasBuilder::Source::suitableForAtlas(int maximumAtlasWidth, int maximumAtlasHeight, int margin)
{
    if (!_image) return false;

    // size too big?
    if (_image->s() + margin * 2 > maximumAtlasWidth) return false;
    if (_image->t() + margin * 2 > maximumAtlasHeight) return false;

    switch (_image->getPixelFormat())
    {
    case(GL_COMPRESSED_ALPHA_ARB):
    case(GL_COMPRESSED_INTENSITY_ARB):
    case(GL_COMPRESSED_LUMINANCE_ALPHA_ARB):
    case(GL_COMPRESSED_LUMINANCE_ARB):
    case(GL_COMPRESSED_RGBA_ARB):
    case(GL_COMPRESSED_RGB_ARB):
    case(GL_COMPRESSED_RGB_S3TC_DXT1_EXT):
    case(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT):
    case(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT):
    case(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT):
        // can't handle compressed textures inside an atlas
        return false;
    default:
        break;
    }

    if ((_image->getPixelSizeInBits() % 8) != 0)
    {
        // pixel size not byte aligned so report as not suitable to prevent other atlas code from having problems with byte boundaries.
        return false;
    }
    if (_texture.valid())
    {

        if (_texture->getWrap(osg::Texture2D::WRAP_S) == osg::Texture2D::REPEAT ||
            _texture->getWrap(osg::Texture2D::WRAP_S) == osg::Texture2D::MIRROR)
        {
            // can't support repeating textures in texture atlas
            return false;
        }

        if (_texture->getWrap(osg::Texture2D::WRAP_T) == osg::Texture2D::REPEAT ||
            _texture->getWrap(osg::Texture2D::WRAP_T) == osg::Texture2D::MIRROR)
        {
            // can't support repeating textures in texture atlas
            return false;
        }

        if (_texture->getReadPBuffer() != 0)
        {
            // pbuffer textures not suitable
            return false;
        }
    }

    return true;
}

osg::Vec2 TextureAtlasBuilder::Source::computeTextureScale() const
{
    if (!_atlas) return osg::Vec2();
    if (!_image) return osg::Vec2();
    if (!(_atlas->_image)) return osg::Vec2();

    typedef osg::Matrix::value_type Float;
    return osg::Vec2(Float(_image->s()) / Float(_atlas->_image->s()), Float(_image->t()) / Float(_atlas->_image->t()));
}

osg::Vec2 TextureAtlasBuilder::Source::computeTextureOffset() const
{
    if (!_atlas) return osg::Vec2();
    if (!_image) return osg::Vec2();
    if (!(_atlas->_image)) return osg::Vec2();

    typedef osg::Matrix::value_type Float;
    return osg::Vec2(Float(_x) / Float(_atlas->_image->s()), Float(_y) / Float(_atlas->_image->t()));
}


osg::Matrix TextureAtlasBuilder::Source::computeTextureMatrix() const
{
    if (!_atlas) return osg::Matrix();
    if (!_image) return osg::Matrix();
    if (!(_atlas->_image)) return osg::Matrix();

    typedef osg::Matrix::value_type Float;
    return osg::Matrix::scale(Float(_image->s()) / Float(_atlas->_image->s()), Float(_image->t()) / Float(_atlas->_image->t()), 1.0) *
        osg::Matrix::translate(Float(_x) / Float(_atlas->_image->s()), Float(_y) / Float(_atlas->_image->t()), 0.0);
}

TextureAtlasBuilder::Atlas::FitsIn TextureAtlasBuilder::Atlas::doesSourceFit(Source* source)
{
    // does the source have a valid image?
    const osg::Image* sourceImage = source->_image.get();
    if (!sourceImage) return DOES_NOT_FIT_IN_ANY_ROW;

    // does pixel format match?
    if (_image.valid())
    {
        if (_image->getPixelFormat() != sourceImage->getPixelFormat()) return DOES_NOT_FIT_IN_ANY_ROW;
        if (_image->getDataType() != sourceImage->getDataType()) return DOES_NOT_FIT_IN_ANY_ROW;
    }

    const osg::Texture2D* sourceTexture = source->_texture.get();
    if (sourceTexture)
    {
        if (sourceTexture->getWrap(osg::Texture2D::WRAP_S) == osg::Texture2D::REPEAT ||
            sourceTexture->getWrap(osg::Texture2D::WRAP_S) == osg::Texture2D::MIRROR)
        {
            // can't support repeating textures in texture atlas
            return DOES_NOT_FIT_IN_ANY_ROW;
        }

        if (sourceTexture->getWrap(osg::Texture2D::WRAP_T) == osg::Texture2D::REPEAT ||
            sourceTexture->getWrap(osg::Texture2D::WRAP_T) == osg::Texture2D::MIRROR)
        {
            // can't support repeating textures in texture atlas
            return DOES_NOT_FIT_IN_ANY_ROW;
        }

        if (sourceTexture->getReadPBuffer() != 0)
        {
            // pbuffer textures not suitable
            return DOES_NOT_FIT_IN_ANY_ROW;
        }

        if (_texture.valid())
        {

            bool sourceUsesBorder = sourceTexture->getWrap(osg::Texture2D::WRAP_S) == osg::Texture2D::CLAMP_TO_BORDER ||
                sourceTexture->getWrap(osg::Texture2D::WRAP_T) == osg::Texture2D::CLAMP_TO_BORDER;

            bool atlasUsesBorder = sourceTexture->getWrap(osg::Texture2D::WRAP_S) == osg::Texture2D::CLAMP_TO_BORDER ||
                sourceTexture->getWrap(osg::Texture2D::WRAP_T) == osg::Texture2D::CLAMP_TO_BORDER;

            if (sourceUsesBorder != atlasUsesBorder)
            {
                // border wrapping does not match
                return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (sourceUsesBorder)
            {
                // border colours don't match
                if (_texture->getBorderColor() != sourceTexture->getBorderColor()) return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (_texture->getFilter(osg::Texture2D::MIN_FILTER) != sourceTexture->getFilter(osg::Texture2D::MIN_FILTER))
            {
                // inconsitent min filters
                return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (_texture->getFilter(osg::Texture2D::MAG_FILTER) != sourceTexture->getFilter(osg::Texture2D::MAG_FILTER))
            {
                // inconsitent mag filters
                return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (_texture->getMaxAnisotropy() != sourceTexture->getMaxAnisotropy())
            {
                // anisotropy different.
                return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (_texture->getInternalFormat() != sourceTexture->getInternalFormat())
            {
                // internal formats inconistent
                return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (_texture->getShadowCompareFunc() != sourceTexture->getShadowCompareFunc())
            {
                // shadow functions inconsitent
                return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (_texture->getShadowTextureMode() != sourceTexture->getShadowTextureMode())
            {
                // shadow texture mode inconsitent
                return DOES_NOT_FIT_IN_ANY_ROW;
            }

            if (_texture->getShadowAmbient() != sourceTexture->getShadowAmbient())
            {
                // shadow ambient inconsitent
                return DOES_NOT_FIT_IN_ANY_ROW;
            }
        }
    }

    if (sourceImage->s() + 2 * _margin > _maximumAtlasWidth)
    {
        // image too big for Atlas
        return DOES_NOT_FIT_IN_ANY_ROW;
    }

    if (sourceImage->t() + 2 * _margin > _maximumAtlasHeight)
    {
        // image too big for Atlas
        return DOES_NOT_FIT_IN_ANY_ROW;
    }

    if ((_y + sourceImage->t() + 2 * _margin) > _maximumAtlasHeight)
    {
        // image doesn't have up space in height axis.
        return DOES_NOT_FIT_IN_ANY_ROW;
    }

    // does the source fit in the current row?
    if ((_x + sourceImage->s() + 2 * _margin) <= _maximumAtlasWidth)
    {
        // yes it fits :-)
        OSG_INFO << "Fits in current row" << std::endl;
        return FITS_IN_CURRENT_ROW;
    }

    // does the source fit in the new row up?
    if ((_height + sourceImage->t() + 2 * _margin) <= _maximumAtlasHeight)
    {
        // yes it fits :-)
        OSG_INFO << "Fits in next row" << std::endl;
        return IN_NEXT_ROW;
    }

    // no space for the texture
    return DOES_NOT_FIT_IN_ANY_ROW;
}

bool TextureAtlasBuilder::Atlas::addSource(Source* source)
{
    // double check source is compatible
    if (!doesSourceFit(source))
    {
        OSG_INFO << "source " << source->_image->getFileName() << " does not fit in atlas " << this << std::endl;
        return false;
    }
    const osg::Image* sourceImage = source->_image.get();
    const osg::Texture2D* sourceTexture = source->_texture.get();

    if (!_image)
    {
        // need to create an image of the same pixel format to store the atlas in
        _image = new osg::Image;
        _image->setPacking(sourceImage->getPacking());
        _image->setPixelFormat(sourceImage->getPixelFormat());
        _image->setDataType(sourceImage->getDataType());
    }

    if (!_texture && sourceTexture)
    {
        _texture = new osg::Texture2D(_image.get());

        _texture->setWrap(osg::Texture2D::WRAP_S, sourceTexture->getWrap(osg::Texture2D::WRAP_S));
        _texture->setWrap(osg::Texture2D::WRAP_T, sourceTexture->getWrap(osg::Texture2D::WRAP_T));

        _texture->setBorderColor(sourceTexture->getBorderColor());
        _texture->setBorderWidth(0);

        _texture->setFilter(osg::Texture2D::MIN_FILTER, sourceTexture->getFilter(osg::Texture2D::MIN_FILTER));
        _texture->setFilter(osg::Texture2D::MAG_FILTER, sourceTexture->getFilter(osg::Texture2D::MAG_FILTER));

        _texture->setMaxAnisotropy(sourceTexture->getMaxAnisotropy());

        _texture->setInternalFormat(sourceTexture->getInternalFormat());

        _texture->setShadowCompareFunc(sourceTexture->getShadowCompareFunc());
        _texture->setShadowTextureMode(sourceTexture->getShadowTextureMode());
        _texture->setShadowAmbient(sourceTexture->getShadowAmbient());

    }

    // now work out where to fit it, first try current row.
    if ((_x + sourceImage->s() + 2 * _margin) <= _maximumAtlasWidth)
    {
        // yes it fits, so add the source to the atlas's list of sources it contains
        _sourceList.push_back(source);

        OSG_INFO << "current row insertion, source " << source->_image->getFileName() << " " << _x << "," << _y << " fits in row of atlas " << this << std::endl;

        // set up the source so it knows where it is in the atlas
        source->_x = _x + _margin;
        source->_y = _y + _margin;
        source->_atlas = this;

        // move the atlas' cursor along to the right
        _x += sourceImage->s() + 2 * _margin;

        if (_x > _width) _width = _x;

        int localTop = _y + sourceImage->t() + 2 * _margin;
        if (localTop > _height) _height = localTop;

        return true;
    }

    // does the source fit in the new row up?
    if ((_height + sourceImage->t() + 2 * _margin) <= _maximumAtlasHeight)
    {
        // now row so first need to reset the atlas cursor
        _x = 0;
        _y = _height;

        // yes it fits, so add the source to the atlas' list of sources it contains
        _sourceList.push_back(source);

        OSG_INFO << "next row insertion, source " << source->_image->getFileName() << " " << _x << "," << _y << " fits in row of atlas " << this << std::endl;

        // set up the source so it knows where it is in the atlas
        source->_x = _x + _margin;
        source->_y = _y + _margin;
        source->_atlas = this;

        // move the atlas' cursor along to the right
        _x += sourceImage->s() + 2 * _margin;

        if (_x > _width) _width = _x;

        _height = _y + sourceImage->t() + 2 * _margin;

        OSG_INFO << "source " << source->_image->getFileName() << " " << _x << "," << _y << " fits in row of atlas " << this << std::endl;

        return true;
    }

    OSG_INFO << "source " << source->_image->getFileName() << " does not fit in atlas " << this << std::endl;

    // shouldn't get here, unless doesSourceFit isn't working...
    return false;
}

void TextureAtlasBuilder::Atlas::clampToNearestPowerOfTwoSize()
{
    int w = 1;
    while (w < _width) w *= 2;

    int h = 1;
    while (h < _height) h *= 2;

    OSG_INFO << "Clamping " << _width << ", " << _height << " to " << w << "," << h << std::endl;

    _width = w;
    _height = h;
}


void TextureAtlasBuilder::Atlas::copySources()
{
    GLenum pixelFormat = _image->getPixelFormat();
    GLenum dataType = _image->getDataType();
    GLenum packing = _image->getPacking();
    OSG_INFO << "Allocated to " << _width << "," << _height << std::endl;
    _image->allocateImage(_width, _height, 1,
        pixelFormat, dataType,
        packing);

    {
        // clear memory
        unsigned int size = _image->getTotalSizeInBytes();
        unsigned char* str = _image->data();
        for (unsigned int i = 0; i < size; ++i) *(str++) = 0;
    }

    OSG_INFO << "Atlas::copySources() " << std::endl;

    for (SourceList::iterator itr = _sourceList.begin();
        itr != _sourceList.end();
        ++itr)
    {
        Source* source = itr->get();
        Atlas* atlas = source->_atlas;

        if (atlas == this)
        {
            OSG_INFO << "Copying image " << source->_image->getFileName() << " to " << source->_x << " ," << source->_y << std::endl;
            OSG_INFO << "        image size " << source->_image->s() << "," << source->_image->t() << std::endl;

            const osg::Image* sourceImage = source->_image.get();
            osg::Image* atlasImage = atlas->_image.get();
            //assert(sourceImage->getPacking() == atlasImage->getPacking()); //Test if packings are equals.
            unsigned int rowSize = sourceImage->getRowSizeInBytes();
            unsigned int pixelSizeInBits = sourceImage->getPixelSizeInBits();
            unsigned int pixelSizeInBytes = pixelSizeInBits / 8;
            unsigned int marginSizeInBytes = pixelSizeInBytes * _margin;

            //assert(atlas->_width  == static_cast<int>(atlasImage->s()));
            //assert(atlas->_height == static_cast<int>(atlasImage->t()));
            //assert(source->_x + static_cast<int>(source->_image->s())+_margin <= static_cast<int>(atlas->_image->s()));        // "+_margin" and not "+2*_margin" because _x already takes the margin into account
            //assert(source->_y + static_cast<int>(source->_image->t())+_margin <= static_cast<int>(atlas->_image->t()));
            //assert(source->_x >= _margin);
            //assert(source->_y >= _margin);
            int x = source->_x;
            int y = source->_y;

            int t;
            for (t = 0; t < sourceImage->t(); ++t, ++y)
            {
                unsigned char* destPtr = atlasImage->data(x, y);
                const unsigned char* sourcePtr = sourceImage->data(0, t);
                for (unsigned int i = 0; i < rowSize; i++)
                {
                    *(destPtr++) = *(sourcePtr++);
                }
            }

            // copy top row margin
            y = source->_y + sourceImage->t();
            int m;
            for (m = 0; m < _margin; ++m, ++y)
            {
                unsigned char* destPtr = atlasImage->data(x, y);
                const unsigned char* sourcePtr = sourceImage->data(0, sourceImage->t() - 1);
                for (unsigned int i = 0; i < rowSize; i++)
                {
                    *(destPtr++) = *(sourcePtr++);
                }

            }



            // copy bottom row margin
            y = source->_y - 1;
            for (m = 0; m < _margin; ++m, --y)
            {
                unsigned char* destPtr = atlasImage->data(x, y);
                const unsigned char* sourcePtr = sourceImage->data(0, 0);
                for (unsigned int i = 0; i < rowSize; i++)
                {
                    *(destPtr++) = *(sourcePtr++);
                }

            }

            // copy left column margin
            y = source->_y;
            for (t = 0; t < sourceImage->t(); ++t, ++y)
            {
                x = source->_x - 1;
                for (m = 0; m < _margin; ++m, --x)
                {
                    unsigned char* destPtr = atlasImage->data(x, y);
                    const unsigned char* sourcePtr = sourceImage->data(0, t);
                    for (unsigned int i = 0; i < pixelSizeInBytes; i++)
                    {
                        *(destPtr++) = *(sourcePtr++);
                    }
                }
            }

            // copy right column margin
            y = source->_y;
            for (t = 0; t < sourceImage->t(); ++t, ++y)
            {
                x = source->_x + sourceImage->s();
                for (m = 0; m < _margin; ++m, ++x)
                {
                    unsigned char* destPtr = atlasImage->data(x, y);
                    const unsigned char* sourcePtr = sourceImage->data(sourceImage->s() - 1, t);
                    for (unsigned int i = 0; i < pixelSizeInBytes; i++)
                    {
                        *(destPtr++) = *(sourcePtr++);
                    }
                }
            }

            // copy top left corner margin
            y = source->_y + sourceImage->t();
            for (m = 0; m < _margin; ++m, ++y)
            {
                unsigned char* destPtr = atlasImage->data(source->_x - _margin, y);
                unsigned char* sourcePtr = atlasImage->data(source->_x - _margin, y - 1); // copy from row below
                for (unsigned int i = 0; i < marginSizeInBytes; i++)
                {
                    *(destPtr++) = *(sourcePtr++);
                }
            }

            // copy top right corner margin
            y = source->_y + sourceImage->t();
            for (m = 0; m < _margin; ++m, ++y)
            {
                unsigned char* destPtr = atlasImage->data(source->_x + sourceImage->s(), y);
                unsigned char* sourcePtr = atlasImage->data(source->_x + sourceImage->s(), y - 1); // copy from row below
                for (unsigned int i = 0; i < marginSizeInBytes; i++)
                {
                    *(destPtr++) = *(sourcePtr++);
                }
            }

            // copy bottom left corner margin
            y = source->_y - 1;
            for (m = 0; m < _margin; ++m, --y)
            {
                unsigned char* destPtr = atlasImage->data(source->_x - _margin, y);
                unsigned char* sourcePtr = atlasImage->data(source->_x - _margin, y + 1); // copy from row below
                for (unsigned int i = 0; i < marginSizeInBytes; i++)
                {
                    *(destPtr++) = *(sourcePtr++);
                }
            }

            // copy bottom right corner margin
            y = source->_y - 1;
            for (m = 0; m < _margin; ++m, --y)
            {
                unsigned char* destPtr = atlasImage->data(source->_x + sourceImage->s(), y);
                unsigned char* sourcePtr = atlasImage->data(source->_x + sourceImage->s(), y + 1); // copy from row below
                for (unsigned int i = 0; i < marginSizeInBytes; i++)
                {
                    *(destPtr++) = *(sourcePtr++);
                }
            }

        }
    }
}
}}//namespace
