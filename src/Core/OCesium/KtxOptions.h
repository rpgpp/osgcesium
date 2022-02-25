#ifndef _KTX_OPTIONS_H__
#define _KTX_OPTIONS_H__

#include "ktx/ktx.h"
#include "ktx/khr_df.h"
#include "ktx/vkformat_enum.h"
#include "image.hpp"
#include <thread>

namespace OC
{
    namespace Cesium
    {

#ifndef GL_SR8
        // From GL_EXT_texture_sRGB_R8
#define GL_SR8                          0x8FBD // same as GL_SR8_EXT
#endif

#ifndef GL_SRG8
// From GL_EXT_texture_sRGB_RG8
#define GL_SRG8                         0x8FBE // same as GL_SRG8_EXT
#endif

#ifndef GL_RGB32F
#define GL_RGB32F                         0x8815
#endif

#ifndef GL_RGBA32F
#define GL_RGBA32F                        0x8814
#endif

// clamp is in std:: from c++17.
#if !(_MSVC_LANG >= 201703L || __cplusplus >= 201703L)
        template <typename T> inline T clamp(T value, T low, T high) {
            return (value < low) ? low : ((value > high) ? high : value);
        }
#endif

        template<typename T>
        struct clampedOption
        {
            clampedOption(T& option, T min_v, T max_v) :
                option(option),
                min(min_v),
                max(max_v)
            {
            }

            void clear()
            {
                option = 0;
            }

            operator T() const
            {
                return option;
            }

            T operator= (T v)
            {
                option = clamp<T>(v, min, max);
                return option;
            }

            T& option;
            T min;
            T max;
        };

        struct basisOptions : public ktxBasisParams {
            // The remaining numeric fields are clamped within the Basis
            // library.
            clampedOption<ktx_uint32_t> threadCount;
            clampedOption<ktx_uint32_t> qualityLevel;
            clampedOption<ktx_uint32_t> maxEndpoints;
            clampedOption<ktx_uint32_t> maxSelectors;
            clampedOption<ktx_uint32_t> uastcRDODictSize;
            clampedOption<float> uastcRDOQualityScalar;
            clampedOption<float> uastcRDOMaxSmoothBlockErrorScale;
            clampedOption<float> uastcRDOMaxSmoothBlockStdDev;

            basisOptions() :
                threadCount(ktxBasisParams::threadCount, 1, 10000),
                qualityLevel(ktxBasisParams::qualityLevel, 1, 255),
                maxEndpoints(ktxBasisParams::maxEndpoints, 1, 16128),
                maxSelectors(ktxBasisParams::maxSelectors, 1, 16128),
                uastcRDODictSize(ktxBasisParams::uastcRDODictSize, 256, 65536),
                uastcRDOQualityScalar(ktxBasisParams::uastcRDOQualityScalar,
                    0.001f, 50.0f),
                uastcRDOMaxSmoothBlockErrorScale(
                    ktxBasisParams::uastcRDOMaxSmoothBlockErrorScale,
                    1.0f, 300.0f),
                uastcRDOMaxSmoothBlockStdDev(
                    ktxBasisParams::uastcRDOMaxSmoothBlockStdDev,
                    0.01f, 65536.0f)
            {
                uint32_t tc = std::thread::hardware_concurrency();
                if (tc == 0) tc = 1;
                threadCount.max = tc;
                threadCount = tc;

                structSize = sizeof(ktxBasisParams);
                // - 1 is to match what basisu_tool does (since 1.13).
                compressionLevel = KTX_ETC1S_DEFAULT_COMPRESSION_LEVEL - 1;
                qualityLevel.clear();
                maxEndpoints.clear();
                endpointRDOThreshold = 0.0f;
                maxSelectors.clear();
                selectorRDOThreshold = 0.0f;
                normalMap = false;
                separateRGToRGB_A = false;
                preSwizzle = false;
                noEndpointRDO = false;
                noSelectorRDO = false;
                uastc = false; // Default to ETC1S.
                uastcRDO = false;
                uastcFlags = KTX_PACK_UASTC_LEVEL_DEFAULT;
                uastcRDODictSize.clear();
                uastcRDOQualityScalar.clear();
                uastcRDODontFavorSimplerModes = false;
                uastcRDONoMultithreading = false;
                noSSE = false;
                verbose = false; // Default to quiet operation.
                for (int i = 0; i < 4; i++) inputSwizzle[i] = 0;
            }
        };

        struct mipgenOptions {
            std::string filter;
            float filterScale;
            enum basisu::Resampler::Boundary_Op wrapMode;

            mipgenOptions() : filter("lanczos4"), filterScale(1.0),
                wrapMode(basisu::Resampler::Boundary_Op::BOUNDARY_CLAMP) { }
        };

        std::string        name;
        std::string        version;
        std::string        defaultVersion;

        void writeId(std::ostream& dst, bool chktest) {
            dst << name << " ";
            dst << (!chktest ? version : defaultVersion);
        }

        inline Image* conv2ktx(osg::Image* inImage)
        {
            unsigned char* imageData = inImage->data();
            int w = inImage->s();
            int h = inImage->t();
            int componentCount = osg::Image::computeNumComponents(inImage->getPixelFormat());
            int componentBits = inImage->getPixelSizeInBits() / componentCount;
            Image* image = nullptr;
            if (componentBits == 16) {
                switch (componentCount) {
                case 1: {
                    image = new r16image(w, h, (r16color*)imageData);
                    break;
                } case 2: {
                    image = new rg16image(w, h, (rg16color*)imageData);
                    break;
                } case 3: {
                    image = new rgb16image(w, h, (rgb16color*)imageData);
                    break;
                } case 4: {
                    image = new rgba16image(w, h, (rgba16color*)imageData);
                    break;
                }
                }
            }
            else {
                switch (componentCount) {
                case 1: {
                    image = new r8image(w, h, (r8color*)imageData);
                    break;
                } case 2: {
                    image = new rg8image(w, h, (rg8color*)imageData);
                    break;
                } case 3: {
                    image = new rgb8image(w, h, (rgb8color*)imageData);
                    break;
                } case 4: {
                    image = new rgba8image(w, h, (rgba8color*)imageData);
                    break;
                }
                }
            }
            switch (componentCount) {
            case 1:
                image->colortype = Image::eLuminance;  // Defined in PNG spec.
                break;
            case 2:
                image->colortype = Image::eLuminanceAlpha; // ditto
                break;
            case 3:
                image->colortype = Image::eRGB;
                break;
            case 4:
                image->colortype = Image::eRGBA;
                break;
            }

            return image;
        }
    }
}

#endif