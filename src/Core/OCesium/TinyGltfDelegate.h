#ifndef OC_TINYGLTF_DELEGATE_H_
#define OC_TINYGLTF_DELEGATE_H_

#include "CesiumDefine.h"
#include "OCMain/osgearth.h"

namespace OC
{
	namespace Cesium
	{
		class TinyGltfDelegate
		{
		public:
			TinyGltfDelegate();
			~TinyGltfDelegate();

            bool writeGltfSceneToFile(String location, bool embedImages, bool embedBuffers, bool prettyPrint, bool isBinary);

            bool writeGltfSceneToStream(std::ostream &stream, bool prettyPrint, bool writeBinary);

			void initScene();

			int pushNode();
			int pushMesh();

            int pushPrimitive(int meshID, osg::PrimitiveSet* pset, int m_materialId, int i_accessorId);
            int setPrimitiveDraco(int meshID, int primitiveId, DracoAttributeMap attr, const char* data, size_t size);
            int pushAccessor(osg::BufferData* data);
            osg::BoundingBox getMinMax(int accessorid);
            void setMinMax(int accessorid, osg::BoundingBox boundingBox);
            void setPrimitiveAttribute(int meshId, int primitiveId, String attribute, int attributeId);

            //materials
            int pushImage(const osg::Image* image, int imageBfv);
            int pushSample(osg::Texture* texture);
            int pushTexture(int imageId, int sampleId);
            int pushMaterials(gltfMaterial* materials, int textureId);
            int pushBufferView(std::stringstream& stream);
            int pushBufferView(BufferType buffertype, const void* data,size_t size);
        protected:
            int pushBufferView(BufferType buffertype, osg::BufferData* data , int target);
            int getBuffer(BufferType bufType);
            std::map<BufferType,int>    mTargetBufferMap;
            tinygltf::Model*            _model;
            //depracate
        private:
            bool isRoot();
            void setRootID(int id);
            void appendChild(int id);
            void appendMatrix(osg::Matrix matrix);
		};
	}
}

#endif



