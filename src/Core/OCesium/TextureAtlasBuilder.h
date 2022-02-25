#ifndef _TEXTURE_ATLAS_BUILDER_H__
#define _TEXTURE_ATLAS_BUILDER_H__

#include "CesiumDefine.h"

namespace OC
{
    namespace Cesium
    {
        class TextureAtlasBuilder : public osg::Referenced
        {
        public:
            TextureAtlasBuilder();

            void reset();

            void setMaximumAtlasSize(int width, int height);

            int getMaximumAtlasWidth() const { return _maximumAtlasWidth; }
            int getMaximumAtlasHeight() const { return _maximumAtlasHeight; }

            void setMargin(int margin);
            int getMargin() const { return _margin; }

            void addSource(const osg::Image* image);
            void addSource(const osg::Texture2D* texture);

            unsigned int getNumSources() const { return _sourceList.size(); }
            const osg::Image* getSourceImage(unsigned int i) { return _sourceList[i]->_image.get(); }
            const osg::Texture2D* getSourceTexture(unsigned int i) { return _sourceList[i]->_texture.get(); }

            void buildAtlas();
            osg::Image* getImageAtlas(unsigned int i);
            osg::Texture2D* getTextureAtlas(unsigned int i);
            osg::Vec2 getTextureOffset(unsigned int i);
            osg::Vec2 getTextureScale(unsigned int i);
            osg::Matrix getTextureMatrix(unsigned int i);

            osg::Image* getImageAtlas(const osg::Image* image);
            osg::Texture2D* getTextureAtlas(const osg::Image* image);
            osg::Vec2 getTextureOffset(const osg::Image* image);
            osg::Vec2 getTextureScale(const osg::Image* image);
            osg::Matrix getTextureMatrix(const osg::Image* image);

            osg::Image* getImageAtlas(const osg::Texture2D* textue);
            osg::Texture2D* getTextureAtlas(const osg::Texture2D* texture);
            osg::Matrix getTextureMatrix(const osg::Texture2D* texture);

        protected:

            int _maximumAtlasWidth;
            int _maximumAtlasHeight;
            int _margin;


            // forward declare
            class Atlas;

            class Source : public osg::Referenced
            {
            public:
                Source() :
                    _x(0), _y(0), _atlas(0) {}

                Source(const osg::Image* image) :
                    _x(0), _y(0), _atlas(0), _image(image) {}

                Source(const osg::Texture2D* texture) :
                    _x(0), _y(0), _atlas(0), _texture(texture) {
                    if (texture) _image = texture->getImage();
                }

                int _x;
                int _y;
                Atlas* _atlas;

                osg::ref_ptr<const osg::Image> _image;
                osg::ref_ptr<const osg::Texture2D> _texture;

                bool suitableForAtlas(int maximumAtlasWidth, int maximumAtlasHeight, int margin);
                osg::Vec2 computeTextureScale() const;
                osg::Vec2 computeTextureOffset() const;
                osg::Matrix computeTextureMatrix() const;


            protected:

                virtual ~Source() {}
            };

            typedef std::vector< osg::ref_ptr<Source> > SourceList;

            class Atlas : public osg::Referenced
            {
            public:
                Atlas(int width, int height, int margin) :
                    _maximumAtlasWidth(width),
                    _maximumAtlasHeight(height),
                    _margin(margin),
                    _x(0),
                    _y(0),
                    _width(0),
                    _height(0),
                    _indexFirstOfRow(0) {}

                int _maximumAtlasWidth;
                int _maximumAtlasHeight;
                int _margin;

                osg::ref_ptr<osg::Texture2D> _texture;
                osg::ref_ptr<osg::Image> _image;

                SourceList _sourceList;

                int _x;
                int _y;
                int _width;
                int _height;
                unsigned int _indexFirstOfRow; ///< Contain the index of the first element of the last row.
                enum FitsIn
                {
                    DOES_NOT_FIT_IN_ANY_ROW,
                    FITS_IN_CURRENT_ROW,
                    IN_NEXT_ROW
                };
                FitsIn doesSourceFit(Source* source);
                bool addSource(Source* source);
                void clampToNearestPowerOfTwoSize();
                void copySources();

            protected:
                virtual ~Atlas() {}
            };

            typedef std::vector< osg::ref_ptr<Atlas> > AtlasList;

            Source* getSource(const osg::Image* image);
            Source* getSource(const osg::Texture2D* texture);

            SourceList _sourceList;
            AtlasList _atlasList;
        private:
            struct CompareSrc
            {
                bool operator()(osg::ref_ptr<Source> src1, osg::ref_ptr<Source> src2) const
                {
                    return src1->_image->t() > src2->_image->t();
                }
            };
            void completeRow(unsigned int indexAtlas);
        };

    }
}

#endif // _TEXTURE_ATLAS_BUILDER_H__

