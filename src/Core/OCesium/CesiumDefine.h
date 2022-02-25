#ifndef _OCesium_DEFINE_H_
#define _OCesium_DEFINE_H_

#if _WIN32
#ifndef OCesium_EXPORTS
#define _CesiumExport  __declspec(dllimport)
#else
#define _CesiumExport  __declspec(dllexport)
#endif
#else
#define _CesiumExport
#endif

#pragma warning(disable:4804)
#pragma warning(disable:4018)

#include "OCUtility/StringUtil.h"
#include "OCMain/json/json.h"
#include "OCMain/osg.h"
#include "OCMain/osgearth.h"
#include "OCLayer/IDatabaseObject.h"

namespace OC
{
	namespace Cesium
	{
		enum BufferType
		{
			BT_POSITION_AND_NORMAL,
			BT_TEXTURE,
			BT_INDICE,
			BT_BATCHID
		};

		enum FeatureBatchType
		{
			FBT_B3DM,
			FBT_I3DM,
			FBT_PNTS,
		};

		class BaseWriter;
		class B3dmWriter;
		class BatchGeode;
		class BatchGroup;
		class CesiumTool;
		class CmptWriter;
		class FeatureBatchTable;
		class FeatureAttributeCallback;
		class I3dmWriter;
		class GLTFObject;
		class GltfHelper;
		class DracoEncoder;
		class gltfConfig;
		class gltfMaterial;
		class PntsWriter;
		class TinyGltfDelegate;
		class TextureAtlasBuilder;
		class WebpEncoder;
		class ObliqueConverter;
		class BuildingAttributeCallback;
		class BuildingConverter;
		class glTFConverter;
		class glTFVisitor;
		class ObliqueConverter;

		struct b3dmheader
		{
			char magic[4];
			unsigned int version;
			unsigned int byteLength;
			unsigned int featureTableJSONByteLength;
			unsigned int featureTableBinaryByteLength;
			unsigned int batchTableJSONByteLength;
			unsigned int batchTableBinaryByteLength;
		};

		struct i3dmheader
		{
			char magic[4];
			unsigned int version;
			unsigned int byteLength;
			unsigned int featureTableJSONByteLength;
			unsigned int featureTableBinaryByteLength;
			unsigned int batchTableJSONByteLength;
			unsigned int batchTableBinaryByteLength;
			unsigned int gltfFormat;
		};

		struct cmptheader
		{
			char magic[4];
			unsigned int version;
			unsigned int byteLength;
			unsigned int tilesLength;
		};

		struct w3dmheader
		{
			char magic[4];
			unsigned int version;
			unsigned int byteLength;
			unsigned int featureTableJSONByteLength;
			unsigned int featureTableBinaryByteLength;
		};

		typedef std::map<String, Json::Value> StringAttributeValues;
		typedef std::map<String, int> DracoAttributeMap;
	}

	inline osg::BoundingBox getBoundBox(osg::Node* node)
	{
		osg::ComputeBoundsVisitor cbv;
		node->accept(cbv);
		return cbv.getBoundingBox();
	}

#define OC_NEW_Tiltset _OC_NewTileset
#define OC_WRITE_Tiltset(t,f) _OC_WriteTileset(t,f)
#define OC_Get_BoundingBox(n) getBoundBox(n)
#define TDTILES OC::Cesium

}

namespace draco
{
	class EncoderBuffer;
	class Mesh;
}

namespace tinygltf
{
	class Model;
	class Node;
	struct Buffer;
	struct BufferView;
}

#endif

