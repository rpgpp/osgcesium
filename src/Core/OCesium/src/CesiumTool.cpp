#include "CesiumTool.h"
#include "GLTFObject.h"
#include "B3dmWriter.h"
#include "OCUtility/StringConverter.h"
#include "OCZip/PakoZip.h"
#include <osgEarth/GeoMath>

namespace OC
{
	namespace Cesium
	{
		CesiumTool::CesiumTool()
		{

		}

		CesiumTool::~CesiumTool() 
		{

		}

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

		osg::Vec4 CesiumTool::encodeFloatRGBA(float v)
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

		float CesiumTool::decodeRGBAFloat(osg::Vec4 rgba)
		{
			return rgba * osg::Vec4(1.0, 1.0 / 255.0f, 1.0 / 65025.0f, 1.0 / 16581375.0f);
		}

		osg::Vec4ub CesiumTool::packFloat(float value)
		{
			osg::Vec4ub result(0,0,0,0);

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
				exponent = floor(Math::Log(value)/ Math::Log(10)) + 1.0;
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

		float CesiumTool::unpackFloat(osg::Vec4ub packedFloat)
		{	
			return unpackFloat(osg::Vec4(
				packedFloat.r(),
				packedFloat.g(),
				packedFloat.b(),
				packedFloat.a()));
		}

		float CesiumTool::unpackFloat(osg::Vec4 packedFloat)
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

		osg::Vec2 CesiumTool::octEncodeInRange(osg::Vec3 normal, float rangeMax)
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

		osg::Vec2 CesiumTool::octEncode(osg::Vec3 vector) {
			return octEncodeInRange(vector, 255);
		};

		float RIGHT_SHIFT = 1.0 / 256.0;
		float LEFT_SHIFT = 256.0;

		osg::Vec4 CesiumTool::octEncodeToVec4(osg::Vec3 vector) 
		{
			osg::Vec4 result;
			osg::Vec2 octEncodeScratch = octEncodeInRange(vector, 65535);
			result.x() = uint8(octEncodeScratch.x() * RIGHT_SHIFT);
			result.y() = uint8(octEncodeScratch.x());
			result.z() = uint8(octEncodeScratch.y() * RIGHT_SHIFT);
			result.w() = uint8(octEncodeScratch.y());
			return result;
		};

		osg::Vec3 CesiumTool::octDecodeInRange(float x, float y, float rangeMax)
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

		osg::Vec3 CesiumTool::octDecode(float x, float y)
		{
			return octDecodeInRange(x,y,255);
		}

		osg::Vec3 CesiumTool::octDecodeFromVec4(osg::Vec4 encoded)
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

		float CesiumTool::width_in_meter(float latitude/*degrees*/, float width/*degrees*/)
		{
			return cos(osg::DegreesToRadians(latitude)) * osg::WGS_84_RADIUS_EQUATOR * osg::PI * (width / 180.0);
		}

		osg::BoundingBox CesiumTool::getBoundBox(osg::Node* node)
		{
			osg::ComputeBoundsVisitor cbv;
			node->accept(cbv);
			return cbv.getBoundingBox();
		}

		osg::BoundingBox CesiumTool::rect2box(CRectangle extent, CVector2 zMinMax)
		{
			osg::BoundingBox box;
			box.xMin() = osg::DegreesToRadians(extent.getMinimum().x);
			box.xMax() = osg::DegreesToRadians(extent.getMaximum().x);
			box.yMin() = osg::DegreesToRadians(extent.getMinimum().y);
			box.yMax() = osg::DegreesToRadians(extent.getMaximum().y);
			box.zMin() = zMinMax.x;
			box.zMax() = zMinMax.y;
			if (osg::equivalent(box.xMin(), box.xMax()))
			{
				box.xMax() += 1e-8;
			}
			if (osg::equivalent(box.yMin(), box.yMax()))
			{
				box.yMax() += 1e-8;
			}
			return box;
		}
		
		float CesiumTool::computeGeometryError(GeoExtent extent)
		{
			return GeoMath::distance(osg::DegreesToRadians(extent.yMin()), osg::DegreesToRadians(extent.xMin()),
				osg::DegreesToRadians(extent.yMax()), osg::DegreesToRadians(extent.xMax()));
		}

		float CesiumTool::computeGeometryError(CRectangle extent)
		{
			CVector2 minimum = extent.getMinimum();
			CVector2 maximun = extent.getMaximum();
			return GeoMath::distance(osg::DegreesToRadians(minimum.y), osg::DegreesToRadians(minimum.x),
				osg::DegreesToRadians(maximun.y), osg::DegreesToRadians(maximun.x));
		}

		bool CesiumTool::writeGLB(osg::Node& node, String location)
		{
			osg::ref_ptr<GLTFObject> gltfModel = new GLTFObject();
			gltfModel->convert(&node);
			return gltfModel->writeToFile(location, true);
		}

		bool CesiumTool::writeGLTF(osg::Node& node, String location)
		{
			osg::ref_ptr<GLTFObject> gltfModel = new GLTFObject();
			gltfModel->convert(&node);
			return gltfModel->writeToFile(location, false);
		}

		bool CesiumTool::writeB3DM(osg::Node& node, String location)
		{
			osg::ref_ptr<B3dmWriter> writer = new B3dmWriter();
			writer->getGltfObject()->convert(&node);

			if (Cesium::gltfConfig::writeGltf)
			{
				writer->getGltfObject()->writeToFile(StringUtil::replaceAll(location, ".b3dm", ".gltf"), false);
			}

			return writer->writeToFile(location);
		}

		bool CesiumTool::writeW3DM(const osg::Image& image, std::ostream& ofs)
		{
			String positionStr, meterStr, usoStr, extentStr, internalFormatStr;
			image.getUserValue(String("position"), positionStr);
			image.getUserValue(String("meter"), meterStr);
			image.getUserValue(String("u_s_o"), usoStr);
			image.getUserValue(String("extent"), extentStr);
			image.getUserValue(String("internalFormat"), internalFormatStr);
			CVector3 position = StringConverter::parseVector3(positionStr);
			CVector3 meter = StringConverter::parseVector3(meterStr);
			CVector3 u_s_o = StringConverter::parseVector3(usoStr);
			CRectangle extent = StringConverter::parseRectangle(extentStr);
			CVector2 mininum = extent.getMinimum();
			CVector2 maxinum = extent.getMaximum();

			osgEarth::Json::Value root = osgEarth::Json::Value(osgEarth::Json::objectValue);

			osgEarth::Json::Value positionV = osgEarth::Json::Value(osgEarth::Json::objectValue);
			positionV["x"] = position.x;
			positionV["y"] = position.y;
			positionV["z"] = position.z;
			root["position"] = positionV;

			osgEarth::Json::Value meterV = osgEarth::Json::Value(osgEarth::Json::objectValue);
			meterV["width"] = meter.x;
			meterV["height"] = meter.y;
			meterV["depth"] = meter.z;
			root["meter"] = meterV;

			osgEarth::Json::Value sourceV = osgEarth::Json::Value(osgEarth::Json::objectValue);
			sourceV["width"] = image.s();
			sourceV["height"] = image.t();
			sourceV["depth"] = image.r();
			root["source"] = sourceV;

			osgEarth::Json::Value u_s_oV = osgEarth::Json::Value(osgEarth::Json::objectValue);
			u_s_oV["scaler"] = u_s_o.x;
			u_s_oV["clampMin"] = u_s_o.y;
			u_s_oV["clampMax"] = u_s_o.z;
			root["u_s_o"] = u_s_oV;

			osgEarth::Json::Value extentV = osgEarth::Json::Value(osgEarth::Json::objectValue);
			extentV["xmin"] = mininum.x;
			extentV["xmax"] = maxinum.x;
			extentV["ymin"] = mininum.y;
			extentV["ymax"] = maxinum.y;
			root["extent"] = extentV;

			root["internalFormat"] = StringConverter::parseInt(internalFormatStr);

			osgEarth::Json::FastWriter writer;
			String mFeatureJSON = writer.write(root);


			w3dmheader header;
			header.magic[0] = 'w', header.magic[1] = '3', header.magic[2] = 'd', header.magic[3] = 'm';
			header.version = 1;

			//json
			header.featureTableJSONByteLength = mFeatureJSON.length();
			ofs.write((const char*)&header, sizeof(header));
			ofs.write(mFeatureJSON.c_str(), header.featureTableJSONByteLength);

			if (ofs.fail())
			{
				return false;
			}

			StringStream sstream;
			size_t writeBeforeSize = ofs.tellp();

			// write image data
			int ps = osg::Image::computeNumComponents(image.getPixelFormat());
			for (int r = 0; r < image.r(); ++r)
			{
				for (int t = 0; t < image.t(); ++t)
				{
					const char* data = (const char*)image.data(0, t, r);
					sstream.write((const char*)data, image.s() * ps);
				}
			}

			bool ret = PakoZip::compress(ofs, sstream.str());

			if (!ret)
			{
				return false;
			}

			header.featureTableBinaryByteLength = ofs.tellp();
			header.featureTableBinaryByteLength -= writeBeforeSize;
			header.byteLength =
				sizeof(w3dmheader) +
				header.featureTableJSONByteLength +
				header.featureTableBinaryByteLength;

			ofs.seekp(std::ios::beg);
			ofs.write((const char*)&header, sizeof(header));
			ofs.seekp(std::ios::end);

			return true;
		}

		bool CesiumTool::writeW3DM(const osg::Image& image, String location)
		{
			osgDB::ofstream ofs(location.c_str(), std::ios::binary);
			bool ret = writeW3DM(image,ofs);
			ofs.close();
			return ret;
		}

	}
}