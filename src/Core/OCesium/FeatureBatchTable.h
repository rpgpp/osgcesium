#ifndef _CESIUM_FEATURE_TABLE_H__
#define _CESIUM_FEATURE_TABLE_H__

#include "FeatureAttributeCallback.h"
#include "OCMain/osgearth.h"

namespace OC
{
    namespace Cesium
    {
        class _CesiumExport FeatureBatchTable : public osg::Referenced
        {
        public:
            FeatureBatchTable(FeatureBatchType type);
            ~FeatureBatchTable();

            FeatureBatchType type();
            void format();
            size_t getFeatureJSONLength();
            size_t getFeatureBodyLength();

            size_t getBatchJSONLength();
            size_t getBatchBodyLength();

            void writeFeatureBody(String key, osg::Array* data);
            void writeBatchBody(String key, osg::Array* data);

            StringStream& featureBody();
            StringStream& batchBody();

            void setFeatureLength(uint32 num);
            uint32 getFeatureLength();
            
            void setBatchLength(uint32 num);
            uint32 getBatchLength();

            void setExternalBytesLength(uint32 length);
            uint32 getExternalBytesLength();

            void setFormat(uint32 format);

            void setRTC_Center(osg::Vec3);
            osg::Vec3 getRTC_Center();

            void setAttributeCallback(FeatureAttributeCallback* callback);
            FeatureAttributeCallback* getAttributeCallback();

            void pushAttribute(String key, int value);
            void pushAttribute(String key, float value);
            void pushAttribute(String key, uint8 value);
            void pushAttribute(String key, uint16 value);
            void pushAttribute(String key, uint32 value);
            void pushAttribute(String key, String value);

            void copyAttributes(FeatureBatchTable* rhs, std::map<uint32, uint32> batchIdMap);
            void writeToStream(std::ostream& output);
            uint32 bytesLength();
        private:
            uint32                                              mGltfFormat = 1;
            uint32                                              mExternalBytesLength = 0;
            uint32                                              mBatchBodyLength = 0;
            uint32                                              mFeatureBodyLength = 0;
            uint32                                              mFeatureLength = 0;
            uint32                                              mBatchLength = 0;
            String                                              mBatchJSON;
            String                                              mFeatureJSON;
            StringStream                                        mBatchBody;
            StringStream                                        mFeatureBody;
            StringAttributeValues	                            mBatchStringAttribute;
            FeatureBatchType                                    mType;
            OC::Json::Value                                     mBatchJSONValue = OC::Json::Value(OC::Json::objectValue);
            OC::Json::Value                                     mFeagtureJSONValue = OC::Json::Value(OC::Json::objectValue);
            std::map<uint32, String>                            mAttributeIndex;
            osgEarth::optional<osg::Vec3>                       mRTC_CENTER;
            osg::ref_ptr<FeatureAttributeCallback>              mAttributeCallback;
            std::map<String, osg::ref_ptr<osg::IntArray> >      mIntAttribute;
            std::map<String, osg::ref_ptr<osg::UIntArray> >     mUIntAttribute;
            std::map<String, osg::ref_ptr<osg::UByteArray> >    mUByteAttribute;
            std::map<String, osg::ref_ptr<osg::UShortArray> >   mUShortAttribute;
            std::map<String, osg::ref_ptr<osg::FloatArray> >    mFloatAttribute;
        };

    }
}
#endif
