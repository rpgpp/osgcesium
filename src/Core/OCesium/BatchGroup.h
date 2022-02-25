#ifndef _Cesium_BATCH_GROUP_H__
#define _Cesium_BATCH_GROUP_H__

#include "BatchGeode.h"
#include "FeatureBatchTable.h"
#include "GltfHelper.h"

namespace OC
{
	namespace Cesium
	{
        class _CesiumExport BatchGroup : public osg::NodeVisitor
        {
        public:
            BatchGroup(FeatureBatchTable* featureBatchTable);
            virtual ~BatchGroup();

            virtual void apply(osg::Geode& node);

            void convert(osg::Node* node);

            BatchGeodeList& getBatchGeodeList(){return mBatchGeodeList;}
            osg::BoundingBox& boundingBox() { return mBoundingBox; }
        protected:
            GltfHelper						                        _gltfHelper;
            void submitImage();
            bool submitBatch();
        private:
            void assignBatch(osg::Geode* geode, osg::Matrix matrix, osg::StateSet* stateset, String referenceName);
            BatchGeodeMap                                           mBatchGeodeMap;
            BatchGeodeList                                          mBatchGeodeList;
            osg::BoundingBox                                        mBoundingBox;
            std::map<String,uint32>                                 mNameBatchId;
            osg::ref_ptr<FeatureBatchTable>                         mFeatureBatchTable;

            //debug info
            std::map<String, int>                                   mImageUseCount;
            int geodeCount = 0;
        };
	}
}


#endif