#include "GltfHelper.h"
#include "OCUtility/StringConverter.h"
#include "gltfMaterial.h"
#include "WebpEncoder.h"
#include "KtxCompress.h"

namespace OC
{
	namespace Cesium
	{
		GltfHelper::GltfHelper()
		{
			_gltfDelegate.initScene();
			_gltfDelegate.pushNode();
			mCurrentMeshId = pushMesh();
		}

		bool GltfHelper::writeGltfSceneToFile(String location, bool embedImages, bool embedBuffers, bool prettyPrint, bool isBinary)
		{
			return _gltfDelegate.writeGltfSceneToFile(location, embedImages, embedBuffers, prettyPrint, isBinary);
		}

		bool GltfHelper::writeGltfSceneToStream(std::ostream& stream, bool writeBinary)
		{
			bool prettyPrint = Cesium::gltfConfig::prettyPrint;
			return _gltfDelegate.writeGltfSceneToStream(stream, prettyPrint, writeBinary);
		}

		int GltfHelper::pushMesh()
		{
			return _gltfDelegate.pushMesh();
		}

		int GltfHelper::getOrCreateAccessor(osg::BufferData* data)
		{
			int accessorId = -1;
			ObjectSequenceMap::iterator it = _accessors.find(data);
			if (it != _accessors.end())
			{
				accessorId = it->second;
			}
			else if (data != NULL)
			{
				accessorId = _gltfDelegate.pushAccessor(data);
				_accessors[data] = accessorId;
			}

			return accessorId;
		}

		int GltfHelper::getOrCreateImageId(gltfMaterial* mat)
		{
			ObjectSequenceMap::iterator it = _imagesIds.find(mat->image);
			if (it != _imagesIds.end())
			{
				return it->second;
			}
			else
			{
				int bfvId = getOrCreateImageBufferView(mat);
				if (bfvId > -1)
				{
					int id = _gltfDelegate.pushImage(mat->image, bfvId);
					_imagesIds[mat->image] = id;
					return id;
				}
			}

			return -1;
		}

		int GltfHelper::getOrCreateImageBufferView(gltfMaterial* mat)
		{
			ObjectSequenceMap::iterator it = _bufferViews.find(mat->image);
			if (it != _bufferViews.end())
			{
				return it->second;
			}

			int bufferViewId = -1;

			if(!mat->imageData.valid())
			{
				mat->imageData = getOrCreateImageData(mat->image);
			}

			if (mat->imageData.valid())
			{
				bufferViewId = _gltfDelegate.pushBufferView(BT_TEXTURE, (const char*)mat->imageData->getDataPointer(), mat->imageData->size());
			}

			_bufferViews[mat->image] = bufferViewId;

			return bufferViewId;
		}

		int GltfHelper::pushMaterial(gltfMaterial* mat)
		{
			ObjectSequenceMap::iterator it = _materialsIds.find(mat);
			if (it != _materialsIds.end())
			{
				return it->second;
			}

			int textureId = -1;
			if (mat->image && mat->texture)
			{
				textureId = getOrCreateTextureId(mat);
			}

			_materialsIds[mat] = mat->materialsId = _gltfDelegate.pushMaterials(mat, textureId);
			return mat->materialsId;
		}

		int GltfHelper::getOrCreateTextureId(gltfMaterial* mat)
		{
			int textureId = -1;
			if (mat->image && mat->texture)
			{
				int imageId = getOrCreateImageId(mat); 
				if (imageId > -1)
				{
					int sampleId = getOrCreateSampler(mat->texture);
					std::stringstream stream;
					stream << imageId;
					stream << sampleId;

					String code = stream.str();
					SamplerSequenceMap::iterator it = _textureMap.find(code);
					if (it != _textureMap.end())
					{
						textureId = it->second;
					}
					else
					{
						textureId = _gltfDelegate.pushTexture(imageId, sampleId);
						_textureMap[code] = textureId;
					}
				}
			}

			return textureId;
		}

		int GltfHelper::getOrCreateSampler(osg::Texture* texture)
		{
			std::stringstream stream;
			stream << texture->getFilter(osg::Texture::MIN_FILTER)
				<< texture->getFilter(osg::Texture::MAG_FILTER)
				<< texture->getWrap(osg::Texture::WRAP_S)
				<< texture->getWrap(osg::Texture::WRAP_T);

			String code = stream.str();

			SamplerSequenceMap::iterator it = _samplerMap.find(code);
			if (it != _samplerMap.end())
			{
				return it->second;
			}

			int id = _gltfDelegate.pushSample(texture);
			return _samplerMap[code] = id;
		}

		int GltfHelper::pushPrimitive(osg::PrimitiveSet* pset, int materialId)
		{
			int accessor = getOrCreateAccessor(pset);
			mCurrentPrimitiveId = _gltfDelegate.pushPrimitive(mCurrentMeshId, pset, materialId, accessor);
			
			return mCurrentPrimitiveId;
		}

		void GltfHelper::setPrimitiveDraco(int primitiveId, DracoAttributeMap attr, const char* data, size_t size)
		{
			_gltfDelegate.setPrimitiveDraco(mCurrentMeshId, mCurrentPrimitiveId, attr, data, size);
		}

		void GltfHelper::pushVertex(int primitiveID, osg::Array* data, osg::BoundingBox boudingBox)
		{
			if (data == NULL)
			{
				return;
			}
			int accessor = pushAttribute("POSITION",data);
			_gltfDelegate.setMinMax(accessor, boudingBox);
		}

		void GltfHelper::pushNormal(int primitiveID, osg::Array* data)
		{
			if (data == NULL)
			{
				return;
			}
			pushAttribute("NORMAL", data);
		}

		void GltfHelper::pushColor(int primitiveID, osg::Array* data)
		{
			if (data == NULL)
			{
				return;
			}
			pushAttribute("COLOR_0", data);
		}

		void GltfHelper::pushTexture(int primitiveID, osg::Array* data)
		{
			if (data == NULL)
			{
				return;
			}
			pushAttribute("TEXCOORD_0", data);
		}

		void GltfHelper::pushBatchId(int primitiveID, osg::Array* data)
		{
			if (data == NULL)
			{
				return;
			}

			pushAttribute("_BATCHID",data);
		}

		int GltfHelper::pushAttribute(String atrribute, osg::Array* data)
		{
			int accessor = getOrCreateAccessor(data);
			_gltfDelegate.setPrimitiveAttribute(mCurrentMeshId, mCurrentPrimitiveId, atrribute , accessor);
			return accessor;
		}

		osg::UByteArray* GltfHelper::getOrCreateImageData(osg::Image* image)
		{
			TextureDataMap::iterator it = _textureDataMap.find(image);
			if (it != _textureDataMap.end())
			{
				return it->second.get();
			}
			else
			{
				osg::ref_ptr<osg::UByteArray> data;
				String ext = osgDB::getLowerCaseFileExtension(image->getFileName());

				std::stringstream stream;
				osgDB::ReaderWriter::WriteResult result;

				if (ext == "rgb" || ext == "")
				{
					ext = "png";
				}
				
				if(ext == "webp")
				{
					osg::ref_ptr<osg::Image> flipped = image;
					int b = image->getPixelSizeInBits() / osg::Image::computeNumComponents(image->getPixelFormat());
					if (b != 8)
					{
						flipped = osgEarth::ImageUtils::convertToRGBA8(image);
					}

					result = WebpEncoder::writeImage(*flipped, stream);
				}
				else if (ext == "ktx" || ext == "ktx2")
				{
					osg::ref_ptr<osg::Image> flipped = image;
					int b = image->getPixelSizeInBits() / osg::Image::computeNumComponents(image->getPixelFormat());
					if (b != 8)
					{
						flipped = osgEarth::ImageUtils::convertToRGBA8(image);
					}

					osg::ref_ptr<osg::UByteArray> data = KtxCompress::writeImage(flipped);
					if (data.valid())
					{
						_textureDataMap[image] = data;
					}
					return data.release();
				}
				else if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(ext))
				{
					osg::ref_ptr<osg::Image> flipped;

					if (ext == "webp")
					{
						flipped = image;
					}
					else
					{
						flipped = new osg::Image(*image);
						flipped->flipVertical();
					}

					result = rw->writeImage(*flipped, stream);
				}

				if (result.success())
				{
					stream.seekp(0, std::ios::end);
					size_t size = stream.tellp();
					stream.seekp(std::ios::beg);

					data = new osg::UByteArray;
					data->resize(size);
					stream.read((char*)data->getDataPointer(), size);
					_textureDataMap[image] = data;
				}

				return data.get();
			}
		}
	}
}
