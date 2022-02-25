#ifndef _CESIUM_TOOL_H_
#define _CESIUM_TOOL_H_

#include "CesiumDefine.h"
#include "gltfConfig.h"
#include "OCUtility/Rectangle.h"
#include "B3dmWriter.h"
#include "I3dmWriter.h"
#include "PntsWriter.h"
#include "CmptWriter.h"
#include "TDTiles.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport CesiumTool
		{
		public:
			CesiumTool();
			~CesiumTool();

			static osg::Vec4 encodeFloatRGBA(float v);
			static float decodeRGBAFloat(osg::Vec4 rgba);
			static osg::Vec4ub packFloat(float value);
			static float unpackFloat(osg::Vec4ub packedFloat);
			static float unpackFloat(osg::Vec4 packedFloat);
			static osg::Vec2 octEncodeInRange(osg::Vec3 normal,float rangeMax);
			static osg::Vec2 octEncode(osg::Vec3 vector);
			static osg::Vec4 octEncodeToVec4(osg::Vec3 vector);
			static osg::Vec3 octDecodeInRange(float x,float y,float rangeMax);
			static osg::Vec3 octDecode(float x, float y);
			static osg::Vec3 octDecodeFromVec4(osg::Vec4 encoded);
			

			static osg::BoundingBox getBoundBox(osg::Node* node);
			static osg::BoundingBox rect2box(CRectangle extent,CVector2 zMinMax = CVector2(0.0,10.0));
			static float computeGeometryError(GeoExtent extent);
			static float computeGeometryError(CRectangle extent/*degrees*/);
			static float width_in_meter(float latitude/*degrees*/, float width/*degrees*/);

			static bool writeGLB(osg::Node& node, String location);
			static bool writeGLTF(osg::Node& node, String location);
			static bool writeB3DM(osg::Node& node, String location);
			static bool writeW3DM(const osg::Image& image, String location);
			static bool writeW3DM(const osg::Image& image, std::ostream& ofs);
		};
	}
}


#endif
