#ifndef _BATCH_GEOMETRY_H__
#define _BATCH_GEOMETRY_H__

#include "CesiumDefine.h"
#include "MergeTriangleOperator.h"
#include "gltfMaterial.h"
#include "DracoEncoder.h"

namespace OC
{
	namespace Cesium
	{
        typedef std::vector<osg::ref_ptr<BatchGeode> > BatchGeodeList;
        typedef std::map<String, osg::ref_ptr<BatchGeode> > BatchGeodeMap;
        class _CesiumExport BatchGeode : public osg::Referenced
        {
        public:
            BatchGeode(gltfMaterial* material);
            ~BatchGeode();

            osg::Vec4Array* textureTransform();
            osg::Vec3Array* positions();
            osg::Vec3Array* normals();
            osg::Vec3Array* colors();
            osg::Vec2Array* textures();
            osg::FloatArray* batchIds();
            osg::PrimitiveSet* primitive();

            gltfMaterial* material();
            DracoEncoder& getDracoEncoder(){return mDracoEncoder;}
            osg::BoundingBox& boundingBox(){return mBoundingBox;}
            void dracoEncode();
            void commit(GltfHelper* gltfHelper);
            osg::Geometry* toGeometry();
            size_t                              getCapacity();
            bool                                mHasErrorTexCoord = false;
            bool                                mHasBatchId = false;
            bool                                mWrapSTOutofRange = false;
            void                                resetBatchId(std::map<uint32, uint32> containBatchIdsMap);
            void                                pushBatchId(uint32 batchId);
            std::map<uint32, uint32>            getContainBatchIdsMap();
            void merge(osg::Geometry* geometry, long batchID, osg::Matrix matrix);
            void merge(BatchGeode* bgeode);
            void transTexCoord(osg::Matrix matrix);
            void setTextureTransform(osg::Vec2 offset,osg::Vec2 scale);
        private:
            bool                                mHasNormals = false;
            bool                                mHasColors = false;
            bool                                mHasTexture = false;
            bool                                mHasTextureTransform = false;
            size_t                              mBytesLength = 0;
            DracoEncoder                        mDracoEncoder;
            osg::BoundingBox                    mBoundingBox;
            GltfTriangleIndexFunctor            mTrifunc;
            std::map<uint32, uint32>            mContainBatchIdsMap;
            GltfTriangleIndexFunctor            mGltfTrifunc;
            osg::ref_ptr<gltfMaterial>          mMaterial;
            osg::ref_ptr<osg::Vec4Array>        mTextureTransform;
            osg::ref_ptr<osg::Vec3Array>        mNormals;
            osg::ref_ptr<osg::Vec3Array>        mColors;
            osg::ref_ptr<osg::Vec3Array>        mPositions;
            osg::ref_ptr<osg::Vec2Array>        mTextureCoord;
            osg::ref_ptr<osg::FloatArray>       mBatchIdArray;
            osg::ref_ptr<osg::UByteArray>       mTextureData;
        };
	}
}

#endif