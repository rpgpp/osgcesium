#include "WebpEncoder.h"
#include "webp/encode.h"
#include "webp/decode.h"
#include "OCUtility/MathUtil.h"

namespace OC
{namespace Cesium{
    enum WebpEncoderOptions
    {
        o_lossless = (1 << 0),
        o_picture = (1 << 1),
        o_photo = (1 << 2),
        o_graph = (1 << 3),
    };

    WebpEncoder::WebpEncoder()
    {

    }

    WebpEncoder::~WebpEncoder()
    {

    }

    int ostream_writer(const uint8_t* data, size_t data_size,
        const WebPPicture* const pic)
    {

        std::ostream* const out = (std::ostream*)pic->custom_ptr;
        return data_size ? (int)out->write((const char*)data, data_size).tellp() : 1;
    }

    osgDB::ReaderWriter::WriteResult WebpEncoder::writeImage(const osg::Image& img, std::ostream& fout)
    {
        int internalFormat = osg::Image::computeNumComponents(img.getPixelFormat());
        int method = 5;
        int quality = 100;

        WebPConfig config;
        config.quality = Math::Clamp(quality, 0, 100);
        config.method = Math::Clamp(method, 0, 6);

      
        unsigned int option;
        option = o_lossless | o_picture;

        if (option & o_lossless)
        {
            config.lossless = 1;
            config.quality = 100;
        }
        if (option & o_picture)
        {
            config.image_hint = WEBP_HINT_PICTURE;
        }

        if (option & o_photo)
        {
            config.image_hint = WEBP_HINT_PHOTO;
        }

        if (option & o_graph)
        {
            config.image_hint = WEBP_HINT_GRAPH;
        }

        WebPPicture picture;
        WebPPreset preset;

        if (!WebPPictureInit(&picture) || !WebPConfigInit(&config))
        {
            return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
        }

        picture.width = img.s();
        picture.height = img.t();

        switch (img.getPixelFormat())
        {
        case (GL_RGB):
            WebPPictureImportRGB(&picture, img.data(), img.getRowSizeInBytes());
            break;
        case (GL_RGBA):
            WebPPictureImportRGBA(&picture, img.data(), img.getRowSizeInBytes());
            break;
        case (GL_LUMINANCE):
            WebPPictureImportRGBX(&picture, img.data(), img.getRowSizeInBytes());
            break;
        default:
            return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            break;
        }

        switch (img.getPixelFormat())
        {
        case (GL_RGB):
        case (GL_RGBA):
            preset = WEBP_PRESET_PHOTO;
            if (!WebPConfigPreset(&config, preset, config.quality))
            {
                WebPPictureFree(&picture);
                return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            }

            if (!WebPValidateConfig(&config))
            {
                WebPPictureFree(&picture);
                return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            }

            picture.writer = ostream_writer;
            picture.custom_ptr = (void*)&fout;
            if (!WebPEncode(&config, &picture))
            {
                WebPPictureFree(&picture);
                return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            }
            break;
        case (GL_LUMINANCE):
            preset = WEBP_PRESET_DEFAULT;
            if (!WebPConfigPreset(&config, preset, config.quality))
            {
                WebPPictureFree(&picture);
                return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            }
            config.lossless = 1;

            if (!WebPValidateConfig(&config))
            {
                WebPPictureFree(&picture);
                return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            }

            picture.writer = ostream_writer;
            picture.custom_ptr = (void*)&fout;
            if (!WebPEncode(&config, &picture))
            {
                WebPPictureFree(&picture);
                return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            }
            break;

        default:
            WebPPictureFree(&picture);
            return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
            break;
        }

        WebPPictureFree(&picture);
        return osgDB::ReaderWriter::WriteResult::FILE_SAVED;
    }


}}//namespace