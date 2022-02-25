#include "RGBAEncoder.h"
#include "MathUtil.h"

namespace OC
{
#define BIAS 38.0
	float SHIFT_LEFT_8 = 256.0;
	float SHIFT_LEFT_16 = 65536.0;
	float SHIFT_LEFT_24 = 16777216.0;

	float SHIFT_RIGHT_8 = 1.0 / SHIFT_LEFT_8;
	float SHIFT_RIGHT_16 = 1.0 / SHIFT_LEFT_16;
	float SHIFT_RIGHT_24 = 1.0 / SHIFT_LEFT_24;

	inline float frac(float value)
	{
		return value - floor(value);
	}

	osg::Vec4 RGBAEncoder::encodeFloatRGBA(float v)
	{
		osg::Vec4 enc = osg::Vec4(1.0f, 255.0f, 65025.0f, 16581375.0f) * v;
		float kEncodeBit = 1.0 / 255.0;
		enc.x() = frac(enc.x());
		enc.y() = frac(enc.y());
		enc.z() = frac(enc.z());
		enc.w() = frac(enc.w());
		enc -= osg::Vec4(enc.y(), enc.z(), enc.w(), enc.w()) * kEncodeBit;
		return enc;
	}

	float RGBAEncoder::decodeRGBAFloat(osg::Vec4 rgba)
	{
		return rgba * osg::Vec4(1.0, 1.0 / 255.0f, 1.0 / 65025.0f, 1.0 / 16581375.0f);
	}

	osg::Vec4ub RGBAEncoder::packFloat(float value)
	{
		osg::Vec4ub result(0, 0, 0, 0);

		if (value == 0.0) {
			return result;
		}

		float sign = value < 0.0 ? 1.0 : 0.0;
		float exponent;
		if (Math::isNaN(value))
		{
			value = 0.1;
			exponent = BIAS;
		}
		else
		{
			value = abs(value);
			exponent = floor(Math::Log(value) / Math::Log(10)) + 1.0;
			value = value / pow(10.0, exponent);
		}

		float temp = value * SHIFT_LEFT_8;
		result.r() = floor(temp);
		temp = (temp - result.r()) * SHIFT_LEFT_8;
		result.g() = floor(temp);
		temp = (temp - result.g()) * SHIFT_LEFT_8;
		result.b() = floor(temp);
		result.a() = (exponent + BIAS) * 2.0 + sign;

		return result;
	}

	float RGBAEncoder::unpackFloat(osg::Vec4ub packedFloat)
	{
		return unpackFloat(osg::Vec4(
			packedFloat.r(),
			packedFloat.g(),
			packedFloat.b(),
			packedFloat.a()));
	}

	float RGBAEncoder::unpackFloat(osg::Vec4 packedFloat)
	{
		float temp = packedFloat.w() / 2.0;
		float exponent = floor(temp);
		float sign = (temp - exponent) * 2.0;
		exponent = exponent - BIAS;

		sign = sign * 2.0 - 1.0;
		sign = -sign;

		if (exponent >= BIAS) {
			return sign < 0.0 ? Math::NEG_INFINITY : Math::POS_INFINITY;
		}

		float unpacked = sign * packedFloat.x() * SHIFT_RIGHT_8;
		unpacked += sign * packedFloat.y() * SHIFT_RIGHT_16;
		unpacked += sign * packedFloat.z() * SHIFT_RIGHT_24;

		return unpacked * pow(10.0, exponent);
	}

	float fromSNorm(float value, float rangeMaximum = 255)
	{
		return (
			(Math::Clamp(value, 0.0f, rangeMaximum) / rangeMaximum) * 2.0 - 1.0
			);
	};

	float toSNorm(float value, float rangeMaximum)
	{
		return round(
			(Math::Clamp(value, -1.0f, 1.0f) * 0.5 + 0.5) * rangeMaximum
		);
	}

	float signNotZero(float value) {
		return value < 0.0 ? -1.0 : 1.0;
	};

	osg::Vec2 RGBAEncoder::octEncodeInRange(osg::Vec3 normal, float rangeMax)
	{
		osg::Vec2 result;
		result.x() =
			normal.x() / (abs(normal.x()) + abs(normal.y()) + abs(normal.z()));
		result.y() =
			normal.y() / (abs(normal.x()) + abs(normal.y()) + abs(normal.z()));
		if (normal.z() < 0) {
			float x = result.x();
			float y = result.y();
			result.x() = (1.0 - abs(y)) * signNotZero(x);
			result.y() = (1.0 - abs(x)) * signNotZero(y);
		}

		result.x() = toSNorm(result.x(), rangeMax);
		result.y() = toSNorm(result.y(), rangeMax);

		return result;
	}

	osg::Vec2 RGBAEncoder::octEncode(osg::Vec3 vector) {
		return octEncodeInRange(vector, 255);
	};

	float RIGHT_SHIFT = 1.0 / 256.0;
	float LEFT_SHIFT = 256.0;

	osg::Vec4 RGBAEncoder::octEncodeToVec4(osg::Vec3 vector)
	{
		osg::Vec4 result;
		osg::Vec2 octEncodeScratch = octEncodeInRange(vector, 65535);
		result.x() = uint8(octEncodeScratch.x() * RIGHT_SHIFT);
		result.y() = uint8(octEncodeScratch.x());
		result.z() = uint8(octEncodeScratch.y() * RIGHT_SHIFT);
		result.w() = uint8(octEncodeScratch.y());
		return result;
	};

	osg::Vec3 RGBAEncoder::octDecodeInRange(float x, float y, float rangeMax)
	{
		osg::Vec3 result;
		result.x() = fromSNorm(x, rangeMax);
		result.y() = fromSNorm(y, rangeMax);
		result.z() = 1.0 - (abs(result.x()) + abs(result.y()));

		if (result.z() < 0.0) {
			float oldVX = result.x();
			result.x() = (1.0 - abs(result.y())) * signNotZero(oldVX);
			result.y() = (1.0 - abs(oldVX)) * signNotZero(result.y());
		}
		result.normalize();
		return result;
	};

	osg::Vec3 RGBAEncoder::octDecode(float x, float y)
	{
		return octDecodeInRange(x, y, 255);
	}

	osg::Vec3 RGBAEncoder::octDecodeFromVec4(osg::Vec4 encoded)
	{
		osg::Vec3 result;
		float x = encoded.x();
		float y = encoded.y();
		float z = encoded.z();
		float w = encoded.w();

		float xOct16 = x * LEFT_SHIFT + y;
		float yOct16 = z * LEFT_SHIFT + w;
		return octDecodeInRange(xOct16, yOct16, 65535);
	}
}