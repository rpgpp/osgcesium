#ifndef _OC_RGBA_ENCODER_H__
#define _OC_RGBA_ENCODER_H__

#include "OCMain/osg.h"
#include "UtilityDefine.h"

namespace OC
{
	class _UtilityExport RGBAEncoder
	{
	public:
		RGBAEncoder() {}
		~RGBAEncoder() {}
		static osg::Vec4 encodeFloatRGBA(float v);
		static float decodeRGBAFloat(osg::Vec4 rgba);
		static osg::Vec4ub packFloat(float value);
		static float unpackFloat(osg::Vec4ub packedFloat);
		static float unpackFloat(osg::Vec4 packedFloat);
		static osg::Vec2 octEncodeInRange(osg::Vec3 normal, float rangeMax);
		static osg::Vec2 octEncode(osg::Vec3 vector);
		static osg::Vec4 octEncodeToVec4(osg::Vec3 vector);
		static osg::Vec3 octDecodeInRange(float x, float y, float rangeMax);
		static osg::Vec3 octDecode(float x, float y);
		static osg::Vec3 octDecodeFromVec4(osg::Vec4 encoded);
	};
}


#endif // !_OC_RGBA_ENCODER_H__
