#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#define TINYGLTF_USE_RAPIDJSON
#pragma warning(disable:4804)
#pragma warning(disable:4018)

#include "tiny_gltf.h"
using namespace tinygltf;

#include "TinyGltfDelegate.h"
#include "gltfMaterial.h"

namespace OC
{
	namespace Cesium
	{
#define PRC2(v) v = (int(v*100))*0.01
		template<class T>
		void alignment_buffer(std::vector<T>& buf) {
			while (buf.size() % 4 != 0) {
				buf.push_back(0x00);
			}
		}

		TinyGltfDelegate::TinyGltfDelegate()
		{
			_model = new tinygltf::Model;
			_model->asset.version = "2.0";
			if (Cesium::gltfConfig::enableDracoCompress)
			{
				_model->extensionsRequired.push_back("KHR_draco_mesh_compression");
				_model->extensionsUsed.push_back("KHR_draco_mesh_compression");
			}
			if (Cesium::gltfConfig::enableWebpCompress)
			{
				_model->extensionsRequired.push_back("EXT_texture_webp");
				_model->extensionsUsed.push_back("EXT_texture_webp");
			}

			//_model->extensionsUsed.push_back("CESIUM_primitive_outline");
		}

		TinyGltfDelegate::~TinyGltfDelegate()
		{
			delete _model;
			_model = NULL;
		}

		int TinyGltfDelegate::pushAccessor(osg::BufferData* data)
		{
			BufferType type;
			int component;
			int count;
			int componentType;

			if (osg::DrawElements* drawElements = dynamic_cast<osg::DrawElements*>(data))
			{
				type = BT_INDICE;
				componentType = TINYGLTF_TYPE_SCALAR;
				component = drawElements->getDataType();
				count = drawElements->getNumIndices();
			}
			else if (osg::Array* array = data->asArray())
			{
				GLint dataSize = array->getDataSize();
				component = array->getDataType();
				componentType = dataSize == 2 ? TINYGLTF_TYPE_VEC2 :
					dataSize == 3 ? TINYGLTF_TYPE_VEC3 :
					dataSize == 4 ? TINYGLTF_TYPE_VEC4 :
					TINYGLTF_TYPE_SCALAR;
				count = array->getNumElements();
				type = BT_POSITION_AND_NORMAL;
			}
			else
			{
				return -1;
			}

			int target =  type == BT_INDICE ? TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER : TINYGLTF_TARGET_ARRAY_BUFFER;


			int bfvId = -1;

			if (!Cesium::gltfConfig::enableDracoCompress)
			{
				bfvId = pushBufferView(type, data, target);
			}

			//create accessor
			int accessorId = _model->accessors.size();
			_model->accessors.push_back(tinygltf::Accessor());
			tinygltf::Accessor* accessor = &_model->accessors.back();

			accessor->bufferView = bfvId;
			accessor->type = componentType;
			accessor->byteOffset = 0;
			accessor->componentType = component;
			accessor->count = count;
			accessor->normalized = false;
			//accessor->normalized = (component == GL_BYTE
			//	|| component == GL_UNSIGNED_BYTE
			//	|| component == GL_SHORT
			//	|| component == GL_UNSIGNED_SHORT);

			return accessorId;
		}

		int TinyGltfDelegate::getBuffer(BufferType type)
		{
			std::map<BufferType, int>::iterator it = mTargetBufferMap.find(type);
			if (it == mTargetBufferMap.end())
			{
				mTargetBufferMap[type] = _model->buffers.size();
				_model->buffers.push_back(tinygltf::Buffer());
			}

			return mTargetBufferMap[type];
		}

		bool TinyGltfDelegate::writeGltfSceneToFile(String location, bool embedImages, bool embedBuffers, bool prettyPrint, bool isBinary)
		{
			tinygltf::TinyGLTF writer;

			return writer.WriteGltfSceneToFile(
				_model,
				location,
				embedImages,			 // embedImages
				embedBuffers,			 // embedBuffers
				prettyPrint,			 // prettyPrint
				isBinary);				 // writeBinary
		}

		bool TinyGltfDelegate::writeGltfSceneToStream(std::ostream &stream,bool prettyPrint, bool writeBinary)
		{
			tinygltf::TinyGLTF writer;
			return writer.WriteGltfSceneToStream(_model, stream, prettyPrint, writeBinary);
		}

		void TinyGltfDelegate::initScene()
		{
			_model->defaultScene = 0;
			_model->scenes.push_back(tinygltf::Scene());
			_model->scenes[_model->defaultScene].nodes.push_back(0);
		}

		bool TinyGltfDelegate::isRoot()
		{
			bool isRoot = _model->scenes[_model->defaultScene].nodes.empty();

			if (isRoot)
			{
				_model->scenes[_model->defaultScene].nodes.push_back(-1);
			}

			return isRoot;
		}

		int TinyGltfDelegate::pushNode()
		{
			_model->nodes.push_back(tinygltf::Node());
			tinygltf::Node& gnode = _model->nodes.back();
			int nodeID = _model->nodes.size() - 1;

			std::stringstream s;
			s << "_gltfNode_" << nodeID;
			gnode.name = s.str();

			return nodeID;
		}

		void TinyGltfDelegate::setRootID(int id)
		{
			_model->scenes[_model->defaultScene].nodes.back() = id;
		}

		void TinyGltfDelegate::appendChild(int id)
		{
			_model->nodes.back().children.push_back(id);
		}

		void TinyGltfDelegate::appendMatrix(osg::Matrix matrix)
		{
			const double* ptr = matrix.ptr();
			for (unsigned i = 0; i < 16; ++i)
			{
				_model->nodes.back().matrix.push_back(*ptr++);
			}
		}

		int TinyGltfDelegate::pushMesh()
		{
			_model->meshes.push_back(tinygltf::Mesh());
			int meshID = _model->meshes.size() - 1;
			_model->nodes.back().mesh = meshID;
			return meshID;
		}

		int TinyGltfDelegate::pushBufferView(BufferType buffertype, osg::BufferData* data, int target)
		{
			int bfvId = pushBufferView(buffertype,data->getDataPointer(),data->getTotalDataSize());

			tinygltf::BufferView& bufferView = _model->bufferViews[bfvId];
			bufferView.target = target;

			return bfvId;
		}

		int TinyGltfDelegate::pushBufferView(std::stringstream& stream)
		{
			_model->buffers.push_back(tinygltf::Buffer());
			tinygltf::Buffer& buffer = _model->buffers.back();
			int bufferId = _model->buffers.size() - 1;

			stream.seekp(0, std::ios::end);
			std::streamsize size = stream.tellp();
			stream.seekp(std::ios::beg);
			buffer.data.resize(size);
			stream.read((char*)&buffer.data[0], size);

			int bfvId = _model->bufferViews.size();
			_model->bufferViews.push_back(tinygltf::BufferView());
			tinygltf::BufferView& bufferView = _model->bufferViews.back();
			bufferView.buffer = bufferId;
			bufferView.byteOffset = 0;
			bufferView.byteLength = _model->buffers[bufferId].data.size();
			bufferView.name = osgEarth::Stringify() << "bufferView_" << bfvId;

			return bfvId;
		}

		int TinyGltfDelegate::pushBufferView(BufferType buffertype,const void* data, size_t size)
		{
#if 1
			int bufferId = 0;
			if (_model->buffers.empty())
			{
				_model->buffers.push_back(tinygltf::Buffer());
			}
#else
			int bufferId = getBuffer(buffertype);
#endif

			int dataPadding = 4 - (size % 4);
			if (dataPadding == 4) dataPadding = 0;

			//write buffer data
			tinygltf::Buffer& buffer = _model->buffers[bufferId];
			size_t byteOffset = buffer.data.size();
			buffer.data.resize(byteOffset + size + dataPadding);
			memcpy(&buffer.data[byteOffset], data, size);
			if(dataPadding!=0)
				memset(&buffer.data[byteOffset + size], 0, dataPadding);

			//gen buffer view
			int bfvId = _model->bufferViews.size();
			_model->bufferViews.push_back(tinygltf::BufferView());
			tinygltf::BufferView& bufferView = _model->bufferViews.back();
			bufferView.buffer = bufferId;
			bufferView.byteOffset = byteOffset;
			bufferView.byteLength = size;
			bufferView.name = osgEarth::Stringify() << "bufferView_" << bfvId;

			return bfvId;
		}

		int TinyGltfDelegate::pushSample(osg::Texture* texture)
		{
			Sampler sampler;
			sampler.minFilter = texture->getFilter(osg::Texture::MIN_FILTER);
			sampler.magFilter = texture->getFilter(osg::Texture::MAG_FILTER);
			sampler.wrapS = texture->getWrap(osg::Texture::WRAP_S);
			sampler.wrapT = texture->getWrap(osg::Texture::WRAP_T);

			if (sampler.wrapS == osg::Texture::CLAMP || sampler.wrapS == osg::Texture::CLAMP_TO_BORDER)
			{
				sampler.wrapS = osg::Texture::CLAMP_TO_EDGE;
			}
			if (sampler.wrapT == osg::Texture::CLAMP || sampler.wrapT == osg::Texture::CLAMP_TO_BORDER)
			{
				sampler.wrapT = osg::Texture::CLAMP_TO_EDGE;
			}

			_model->samplers.push_back(sampler);

			return (int)_model->samplers.size() - 1;
		}

		int TinyGltfDelegate::pushPrimitive(int meshID, osg::PrimitiveSet* pset
			, int m_materialId, int i_accessorId)
		{
			tinygltf::Mesh& mesh = _model->meshes[meshID];
			mesh.primitives.push_back(tinygltf::Primitive());

			tinygltf::Primitive& primitive = mesh.primitives.back();
			primitive.mode = pset->getMode();
			primitive.material = m_materialId;
			primitive.indices = i_accessorId;

			//Value::Object v;
			//v["indices"] = Value(i_accessorId);
			//primitive.extensions["CESIUM_primitive_outline"] = Value(v);

			return mesh.primitives.size() - 1;
		}

		int TinyGltfDelegate::setPrimitiveDraco(int meshID, int primitiveId, DracoAttributeMap attr, const char* data, size_t size)
		{
			if (!Cesium::gltfConfig::enableDracoCompress)
			{
				return -1;
			}

			tinygltf::Mesh& mesh = _model->meshes[meshID];
			tinygltf::Primitive& primitive = mesh.primitives[primitiveId];

			Value::Object dracoAttirubtes;
			Value::Object v;
			for (auto i : attr)
			{
				dracoAttirubtes[i.first] = Value(i.second);
			}

			int bfv = pushBufferView(BT_POSITION_AND_NORMAL, data, size);
			v["bufferView"] = Value(bfv);
			v["attributes"] = Value(dracoAttirubtes);
			primitive.extensions["KHR_draco_mesh_compression"] = Value(v);
			
			return 1;
		}

		void TinyGltfDelegate::setPrimitiveAttribute(int meshId, int primitiveId, String attribute, int attributeId)
		{
			if (attributeId > -1)
			{
				tinygltf::Mesh& mesh = _model->meshes[meshId];
				tinygltf::Primitive& primitive = mesh.primitives[primitiveId];
				primitive.attributes[attribute] = attributeId;
			}
		}

		osg::BoundingBox TinyGltfDelegate::getMinMax(int accessorid)
		{
			osg::BoundingBox boundingBox;
			boundingBox.xMin() = _model->accessors[accessorid].minValues[0];
			boundingBox.yMin() = _model->accessors[accessorid].minValues[1];
			boundingBox.zMin() = _model->accessors[accessorid].minValues[2];
			boundingBox.xMax() = _model->accessors[accessorid].maxValues[0];
			boundingBox.yMax() = _model->accessors[accessorid].maxValues[1];
			boundingBox.zMax() = _model->accessors[accessorid].maxValues[2];
			return boundingBox;
		}

		void TinyGltfDelegate::setMinMax(int accessorid, osg::BoundingBox boundingBox)
		{
			tinygltf::Accessor& posacc = _model->accessors[accessorid];

			if (posacc.minValues.empty())
			{
				posacc.minValues.push_back(boundingBox.xMin());
				posacc.minValues.push_back(boundingBox.yMin());
				posacc.minValues.push_back(boundingBox.zMin());
			}

			if (posacc.maxValues.empty())
			{
				posacc.maxValues.push_back(boundingBox.xMax());
				posacc.maxValues.push_back(boundingBox.yMax());
				posacc.maxValues.push_back(boundingBox.zMax());
			}
		}

		int TinyGltfDelegate::pushMaterials(gltfMaterial* materials, int textureId)
		{
			Material mat;

			mat.pbrMetallicRoughness.baseColorTexture.index = textureId;
			mat.doubleSided = materials->doubleSide || Cesium::gltfConfig::forceDoubleSide;

			if (materials->offset != osg::Vec2(0.0, 0.0)
				|| materials->scale != osg::Vec2(1.0, 1.0))
			{
				Value::Array offset;
				offset.push_back(Value(materials->offset.x()));
				offset.push_back(Value(materials->offset.y()));
				Value::Array scale;
				scale.push_back(Value(materials->scale.x()));
				scale.push_back(Value(materials->scale.y()));

				Value::Object v;
				v["offset"] = Value(offset);
				v["scale"] = Value(scale);

				mat.pbrMetallicRoughness.baseColorTexture.extensions["KHR_texture_transform"] = Value(v);
			}
			
			osg::Vec4 diffuse = materials->diffuse;
			osg::Vec4 ambient = materials->ambient;
			osg::Vec4 specular = materials->specular;
			if (materials->blend)
			{
				mat.alphaMode = "BLEND";
			}

			mat.pbrMetallicRoughness.roughnessFactor = materials->roughnessFactor;
			mat.pbrMetallicRoughness.metallicFactor = materials->metallicFactor;

			mat.pbrMetallicRoughness.baseColorFactor =
				std::vector<double>{ PRC2(diffuse.x()) ,PRC2(diffuse.y()) ,PRC2(diffuse.z()) ,PRC2(diffuse.w()) };

			if (materials->KHR_materials_unlit)
			{
				mat.extensions["KHR_materials_unlit"] = Value("");
			}

			if (mat.pbrMetallicRoughness.baseColorTexture.index == -1
				&& mat.pbrMetallicRoughness.roughnessFactor == 1.0
				&& mat.pbrMetallicRoughness.metallicFactor == 1.0
				&& mat.pbrMetallicRoughness.baseColorFactor[0] == 1.0
				&& mat.pbrMetallicRoughness.baseColorFactor[1] == 1.0
				&& mat.pbrMetallicRoughness.baseColorFactor[2] == 1.0
				&& mat.pbrMetallicRoughness.baseColorFactor[3] == 1.0
				&& materials->blend == false)
			{
				return -1;
			}

			_model->materials.push_back(mat);
			return _model->materials.size() - 1;
		}

		int TinyGltfDelegate::TinyGltfDelegate::pushImage(const osg::Image* osgImage,int imageBfv)
		{
			if (imageBfv > -1)
			{
				String ext = osgDB::getLowerCaseFileExtension(osgImage->getFileName());
				String filename = "";
				tinygltf::Image image;
				if (ext == "jpg" || ext == "jpeg")
				{
					image.mimeType = "image/jpeg";
				}
				else if (ext == "ktx")
				{
					image.mimeType = "image/ktx";
				}
				else if (ext == "ktx2")
				{
					image.mimeType = "image/ktx2";
				}
				else  if (ext == "webp")
				{
					image.mimeType = "image/webp";
				}
				else/*  if (ext == "png")*/
				{
					image.mimeType = "image/png";
				}
				
				image.uri = filename;
				image.bufferView = imageBfv;
				_model->images.push_back(image);

				return _model->images.size() - 1;
			}

			return -1;
		}

		int TinyGltfDelegate::pushTexture(int imageId, int sampleId) 
		{
			Texture texture;
			texture.source = imageId;
			texture.sampler = sampleId;
			_model->textures.push_back(texture);
			return _model->textures.size() - 1;
		}
	}
}