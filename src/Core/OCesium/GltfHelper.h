#ifndef _OC_GLTFHELPER_H_
#define _OC_GLTFHELPER_H_

#include "CesiumDefine.h"
#include "TinyGltfDelegate.h"

namespace OC
{
	namespace Cesium
	{
		class GltfHelper
		{
		private:
			typedef std::map<String, int> SamplerSequenceMap;
			typedef std::map<osg::ref_ptr<const osg::Object>, int> ObjectSequenceMap;
			typedef std::map<osg::ref_ptr<const osg::Object>, osg::ref_ptr<osg::UByteArray> > TextureDataMap;

			TextureDataMap		_textureDataMap;
			ObjectSequenceMap	_bufferViews;
			ObjectSequenceMap	_materialsIds;
			ObjectSequenceMap	_imagesIds;
			ObjectSequenceMap	_accessors;
			SamplerSequenceMap	_samplerMap;
			SamplerSequenceMap	_textureMap;
		public:
			GltfHelper();

			bool writeGltfSceneToFile(String location, bool embedImages, bool embedBuffers, bool prettyPrint, bool isBinary);

			bool writeGltfSceneToStream(std::ostream& stream,bool writeBinary);
			int pushMaterial(gltfMaterial* mat);
			int pushPrimitive(osg::PrimitiveSet* pset, int materialId);
			void setPrimitiveDraco(int primitiveId, DracoAttributeMap attr, const char* data, size_t size);
			void pushVertex(int primitiveID, osg::Array* data, osg::BoundingBox boudingBox);
			void pushNormal(int primitiveID, osg::Array* data);
			void pushColor(int primitiveID, osg::Array* data);
			void pushTexture(int primitiveID, osg::Array* data);
			void pushBatchId(int primitiveID, osg::Array* data);
			int pushAttribute(String atrribute, osg::Array* data);
			osg::UByteArray* getOrCreateImageData(osg::Image* image);
		private:
			int pushMesh();
			int mCurrentMeshId = -1;
			int mCurrentPrimitiveId = -1;
			int getOrCreateAccessor(osg::BufferData* data);
			int getOrCreateTextureId(gltfMaterial* mat);
			int getOrCreateSampler(osg::Texture* texture);
			int getOrCreateImageId(gltfMaterial* mat);
			int getOrCreateImageBufferView(gltfMaterial* mat);
			TinyGltfDelegate				_gltfDelegate;
		};
	}
}


#endif
