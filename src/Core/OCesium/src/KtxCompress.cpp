#include "KtxCompress.h"
#include "KtxOptions.h"

namespace OC {
    namespace Cesium
    {  
        KtxCompress::KtxCompress()
        {
        
        }

        KtxCompress::~KtxCompress()
        {

        }

        osg::UByteArray* KtxCompress::writeImage(osg::Image* inImage)
        {
            basisOptions bopts;
            bopts.uastc = true;
            bopts.threadCount = 10;

            mipgenOptions gmopts;

            bool ktx2 = true;
            int metadata = 1;
            int lower_left_maps_to_s0t0 = 1;
            bool automipmap = false;
            bool mipmap = false;
            bool genmipmap = false;
            int levels = 0;
            bool bcmp = false;
            bool zcmp = true;
            bool srgb = true;
            String defaultSwizzle = "";

            osg::ref_ptr<osg::UByteArray> data;

            Image* image = conv2ktx(inImage);

            if (image->getColortype() < Image::colortype_e::eR) {  // Luminance type?
                if (image->getColortype() == Image::colortype_e::eLuminance) {
                    defaultSwizzle = "rrr1";
                }
                else if (image->getColortype() == Image::colortype_e::eLuminanceAlpha) {
                    defaultSwizzle = "rrrg";
                }
            }

            if (image->getHeight() > 1 && lower_left_maps_to_s0t0 && !bopts.uastc) {
                image->yflip();
            }

            KTX_error_code ret;
            ktxTextureCreateInfo createInfo;
            ktxTexture* texture = 0;

            unsigned int componentCount = 1, faceSlice, level, layer, levelCount = 1;
            unsigned int levelWidth = 0, levelHeight = 0, levelDepth = 0;
            khr_df_transfer_e chosenOETF = KHR_DF_TRANSFER_UNSPECIFIED;
            khr_df_transfer_e firstImageOETF = KHR_DF_TRANSFER_UNSPECIFIED;
            khr_df_primaries_e chosenPrimaries = KHR_DF_PRIMARIES_UNSPECIFIED;
            khr_df_primaries_e firstImagePrimaries = KHR_DF_PRIMARIES_UNSPECIFIED;

            faceSlice = layer = level = 0;

            memset(&createInfo, 0, sizeof(createInfo));
            createInfo.numFaces = 1;
            createInfo.numLayers = 1;
            createInfo.isArray = KTX_FALSE;

            GLenum pixelFormat = inImage->getPixelFormat();
            componentCount = osg::Image::computeNumComponents(pixelFormat);

            int componentBits = image->getComponentSize();

            switch (componentCount) {
            case 1:
                switch (componentBits) {
                case 1:
                    createInfo.glInternalformat
                        = srgb ? GL_SR8 : GL_R8;
                    createInfo.vkFormat
                        = srgb ? VK_FORMAT_R8_SRGB
                        : VK_FORMAT_R8_UNORM;
                    break;
                case 2:
                    createInfo.glInternalformat = GL_R16;
                    createInfo.vkFormat = VK_FORMAT_R16_UNORM;
                    break;
                case 4:
                    createInfo.glInternalformat = GL_R32F;
                    createInfo.vkFormat = VK_FORMAT_R32_SFLOAT;
                    break;
                }
                break;

            case 2:
                switch (componentBits) {
                case 1:
                    createInfo.glInternalformat
                        = srgb ? GL_SRG8 : GL_RG8;
                    createInfo.vkFormat
                        = srgb ? VK_FORMAT_R8G8_SRGB
                        : VK_FORMAT_R8G8_UNORM;
                    break;
                case 2:
                    createInfo.glInternalformat = GL_RG16;
                    createInfo.vkFormat = VK_FORMAT_R16G16_UNORM;
                    break;
                case 4:
                    createInfo.glInternalformat = GL_RG32F;
                    createInfo.vkFormat = VK_FORMAT_R32G32_SFLOAT;
                    break;
                }
                break;

            case 3:
                switch (componentBits) {
                case 1:
                    createInfo.glInternalformat
                        = srgb ? GL_SRGB8 : GL_RGB8;
                    createInfo.vkFormat
                        = srgb ? VK_FORMAT_R8G8B8_SRGB
                        : VK_FORMAT_R8G8B8_UNORM;
                    break;
                case 2:
                    createInfo.glInternalformat = GL_RGB16;
                    createInfo.vkFormat = VK_FORMAT_R16G16B16_UNORM;
                    break;
                case 4:
                    createInfo.glInternalformat = GL_RGB32F;
                    createInfo.vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
                    break;
                }
                break;

            case 4:
                switch (componentBits) {
                case 1:
                    createInfo.glInternalformat
                        = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
                    createInfo.vkFormat
                        = srgb ? VK_FORMAT_R8G8B8A8_SRGB
                        : VK_FORMAT_R8G8B8A8_UNORM;
                    break;
                case 2:
                    createInfo.glInternalformat = GL_RGBA16;
                    createInfo.vkFormat = VK_FORMAT_R16G16B16A16_UNORM;
                    break;
                case 4:
                    createInfo.glInternalformat = GL_RGBA32F;
                    createInfo.vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
                    break;
                }
                break;

            default:
                /* If we get here there's a bug. */
                assert(0);
            }

            if (createInfo.vkFormat == VK_FORMAT_R8_SRGB
                || createInfo.vkFormat == VK_FORMAT_R8G8_SRGB) {
                std::cout << ("GPU support of sRGB variants of R & RG formats is"
                    " limited.\nConsider using '--convert_oetf linear'"
                    " to avoid these formats.");
            }

            createInfo.baseWidth = levelWidth = inImage->s();
            createInfo.baseHeight = levelHeight = inImage->t();
            createInfo.baseDepth = levelDepth = inImage->r();
            if (inImage->t() == 1)
                createInfo.numDimensions = 1;
            else
                createInfo.numDimensions = 2;

            if (automipmap)
            {
                createInfo.numLevels = 1;
                createInfo.generateMipmaps = KTX_TRUE;
            }
            else
            {
                createInfo.generateMipmaps = KTX_FALSE;
                if (mipmap || genmipmap) {
                    // Calculate number of miplevels
                    GLuint max_dim = inImage->s() > inImage->t() ?
                        inImage->s() : inImage->t();
                    createInfo.numLevels = log2(max_dim) + 1;
                    if (levels > 1) {
                        if (levels > createInfo.numLevels) {
                            std::cerr << /*name <<*/ "--levels value is greater than "
                                << "the maximum levels for the image size."
                                << std::endl;
                        }
                        // Override the above.
                        createInfo.numLevels = levels;
                    }
                }
                else {
                    createInfo.numLevels = 1;
                }
                // Figure out how many levels we'll read from files.
                if (mipmap) {
                    levelCount = createInfo.numLevels;
                }
                else {
                    levelCount = 1;
                }
            }

            if (ktx2) {
                ret = ktxTexture2_Create(&createInfo,
                    KTX_TEXTURE_CREATE_ALLOC_STORAGE,
                    (ktxTexture2**)&texture);
            }
            else {
                ret = ktxTexture1_Create(&createInfo,
                    KTX_TEXTURE_CREATE_ALLOC_STORAGE,
                    (ktxTexture1**)&texture);
            }

            ktxTexture_SetImageFromMemory(ktxTexture(texture),
                level,
                layer,
                faceSlice,
                *image,
                image->getByteCount());

            if (genmipmap) {
                for (uint32_t glevel = 1; glevel < createInfo.numLevels; glevel++)
                {
                    Image* levelImage = image->createImage(
                        maximum<uint32_t>(1, image->getWidth() >> glevel),
                        maximum<uint32_t>(1, image->getHeight() >> glevel));
                    levelImage->setOetf(image->getOetf());
                    levelImage->setColortype(image->getColortype());
                    levelImage->setPrimaries(image->getPrimaries());
                    try {
                        image->resample(*levelImage,
                            image->getOetf() == KHR_DF_TRANSFER_SRGB,
                            gmopts.filter.c_str(),
                            gmopts.filterScale,
                            gmopts.wrapMode);
                    }
                    catch (...) {
                        std::cerr << name << ": Image::resample() failed! " << std::endl;
                    }

                    // TODO: add an option for renormalize;
                    //if (options.gmopts.mipRenormalize)
                    //    levelImage->renormalize_normal_map();

                    ktxTexture_SetImageFromMemory(ktxTexture(texture),
                        glevel,
                        layer,
                        faceSlice,
                        *levelImage,
                        levelImage->getByteCount());
                    delete levelImage;
                }
            }

            delete image;

            //
            /*
            * Add orientation metadata.
            * Note: 1D textures and 2D textures with a height of 1 don't need
            * orientation metadata
            */
            if (metadata && createInfo.baseHeight > 1) {
                ktxHashList* ht = &texture->kvDataHead;
                char orientation[10];
                if (ktx2) {
                    orientation[0] = 'r';
                    orientation[1] = lower_left_maps_to_s0t0 ? 'u' : 'd';
                    orientation[2] = 0;
                }
                else {
                    assert(strlen(KTX_ORIENTATION2_FMT) < sizeof(orientation));

                    snprintf(orientation, sizeof(orientation), KTX_ORIENTATION2_FMT,
                        'r', lower_left_maps_to_s0t0 ? 'u' : 'd');
                }
                ktxHashList_AddKVPair(ht, KTX_ORIENTATION_KEY,
                    (unsigned int)strlen(orientation) + 1,
                    orientation);
            }


            if (ktx2) {
                // Add required writer metadata.
                std::stringstream writer;
                writeId(writer, true);
                ktxHashList_AddKVPair(&texture->kvDataHead, KTX_WRITER_KEY,
                    (ktx_uint32_t)writer.str().length() + 1,
                    writer.str().c_str());

                std::string swizzle;
                // Add Swizzle metadata
                if (!bcmp && !bopts.uastc
                    && defaultSwizzle.size()) {
                    swizzle = defaultSwizzle;
                }
                if (swizzle.size()) {
                    ktxHashList_AddKVPair(&texture->kvDataHead, KTX_SWIZZLE_KEY,
                        (uint32_t)swizzle.size() + 1,
                        // +1 is for the NUL on the c_str
                        swizzle.c_str());
                }

                if (ktx2 && chosenPrimaries != KHR_DF_PRIMARIES_BT709) {
                    KHR_DFDSETVAL(((ktxTexture2*)texture)->pDfd + 1, PRIMARIES,
                        chosenPrimaries);
                }
            }
            
            ret = ktxTexture2_CompressBasisEx((ktxTexture2*)texture, &bopts);

            int zcmpLevel = 22;
            if (zcmp) {
                ret = ktxTexture2_DeflateZstd((ktxTexture2*)texture,zcmpLevel);
            }

            String filename = inImage->getFileName();
            String imagename,ext,path;
            StringUtil::splitFullFilename(filename, imagename, ext, path);

            ktx_uint8_t* ptr = NULL;
            ktx_size_t size = 0;
            ktxTexture_WriteToMemory(ktxTexture(texture),&ptr,&size);

            if (ptr && size > 0)
            {
                data = new osg::UByteArray;
                data->resize(size);
                memcpy((ktx_uint8_t*)data->getDataPointer(), ptr, size);

                //String outfilename = "d:/" + imagename + ".ktx2";
                //std::ofstream ofs(outfilename.c_str(), std::ios::binary);
                //ofs.write((const char*)ptr, size);
                //ofs.close();

                CORE_SAFE_DELETE(ptr);
            }

            if (texture) ktxTexture_Destroy(ktxTexture(texture));

            return data.release();
        }

} }//namespace