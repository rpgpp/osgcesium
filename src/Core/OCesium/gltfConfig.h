#ifndef _GLTF_CONFIG_DEFINE_H__
#define _GLTF_CONFIG_DEFINE_H__

#include "CesiumDefine.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport gltfConfig
		{
		public:
			static int sectionSize;
			static int quantizationbits;
			static int AtlasImageSize;
			static int MaxTextureSize;
			static int TextrueCoordError;
			static bool GeodePerId;
			static bool writeGltf;
			static bool prettyPrint;
			static bool mergeClampTexture;
			static bool mergeRepeatTexture;
			static bool disableLight;
			static bool enableWebpCompress;
			static bool enableKtxCompress;
			static bool enableDracoCompress;
			static bool forceDoubleSide;
			static float geometryErroRatio;
			static float metallicFactor;
			static float roughnessFactor;
		};
	}
}

#endif // !
