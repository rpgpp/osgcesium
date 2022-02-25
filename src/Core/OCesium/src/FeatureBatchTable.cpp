#include "FeatureBatchTable.h"

namespace OC{namespace Cesium{

    FeatureBatchTable::FeatureBatchTable(FeatureBatchType type)
        :mType(type)
    {

    }

    FeatureBatchTable::~FeatureBatchTable()
    {

    }

    void FeatureBatchTable::setBatchLength(uint32 num)
    {
        mBatchLength = num;
    }

    uint32 FeatureBatchTable::getBatchLength()
    {
        return mBatchLength;
    }

    void FeatureBatchTable::setExternalBytesLength(uint32 length)
    {
        mExternalBytesLength = length;
    }

    uint32 FeatureBatchTable::getExternalBytesLength()
    {
        return mExternalBytesLength;
    }

    void FeatureBatchTable::setFormat(uint32 format)
    {
        mGltfFormat = format;
    }

    void FeatureBatchTable::setFeatureLength(uint32 num)
    {
        mFeatureLength = num;
    }

    uint32 FeatureBatchTable::getFeatureLength()
    {
        return mFeatureLength;
    }

    void FeatureBatchTable::setRTC_Center(osg::Vec3 center)
    {
        mRTC_CENTER = center;
    }

    osg::Vec3 FeatureBatchTable::getRTC_Center()
    {
        return mRTC_CENTER.get();
    }

    void FeatureBatchTable::setAttributeCallback(FeatureAttributeCallback* callback)
    {
        mAttributeCallback = callback;
    }

    FeatureAttributeCallback* FeatureBatchTable::getAttributeCallback()
    {
        return mAttributeCallback.get();
    }

    FeatureBatchType FeatureBatchTable::type()
    {
        return mType;
    }

    void FeatureBatchTable::format()
    {
        if (!mFeatureJSON.empty())
        {
            //already format
            return;
        }

        //FeatureJson
        {
            Json::FastWriter writer;
            if (mType == FBT_B3DM)
            {
                mFeagtureJSONValue["BATCH_LENGTH"] = mBatchLength;
            }
            else if (mType == FBT_I3DM)
            {
                mFeagtureJSONValue["INSTANCES_LENGTH"] = mFeatureLength;
            }
            else
            {
                if (mBatchLength > 0)
                {
                    mFeagtureJSONValue["BATCH_LENGTH"] = mBatchLength;
                }
                mFeagtureJSONValue["POINTS_LENGTH"] = mFeatureLength;
            }

            if (mRTC_CENTER.isSet())
            {
                Json::Value center(Json::arrayValue);
                center.append(mRTC_CENTER.value().x());
                center.append(mRTC_CENTER.value().y());
                center.append(mRTC_CENTER.value().z());
                mFeagtureJSONValue["RTC_CENTER"] = center;
            }

            mFeatureJSON = writer.write(mFeagtureJSONValue);
            while (mFeatureJSON.length() % 4 != 0)
            {
                mFeatureJSON.push_back(' ');
            }
            if (mFeatureBodyLength > 0)
            {
                int padding = 4 - (mFeatureBodyLength % 4);
                if (padding == 4) padding = 0;
                mFeatureBody.write("\0\0\0", padding);
                mFeatureBodyLength += padding;
            }
        }

        //BatchJson
        {
            for (auto& pair : mBatchStringAttribute)
            {
                Json::Value values = pair.second;
                if ((uint32)values.size() == mBatchLength)
                {
                    String key = pair.first;
                    mBatchJSONValue[key] = values;
                }
            }

            for (auto i : mFloatAttribute)
            {
                writeBatchBody(i.first, i.second);
            }
            for (auto i : mIntAttribute)
            {
                writeBatchBody(i.first, i.second);
            }
            for (auto i : mUIntAttribute)
            {
                writeBatchBody(i.first, i.second);
            }

            if (mBatchJSONValue.size() > 0)
            {
                int count = 0;
                StringStream sstream;
                sstream << "{";
                for (auto i : mAttributeIndex)
                {
                    String name = i.second;
                    if (name.empty())
                    {
                        continue;
                    }
                    if (count > 0)
                    {
                        sstream << ",";
                    }
                    sstream << "\"" << name << "\":";
                    Json::FastWriter writer;
                    sstream << writer.write(mBatchJSONValue[name]);
                    count++;
                }

                sstream << "}";
                mBatchJSON = sstream.str();
                while (mBatchJSON.length() % 4 != 0)
                {
                    mBatchJSON.push_back(' ');
                }
            }

            if (mBatchBodyLength > 0)
            {
                int padding = 4 - (mBatchBodyLength % 4);
                if (padding == 4) padding = 0;
                mBatchBody.write("   ", padding);
                mBatchBodyLength += padding;
            }
        }
    }

    size_t FeatureBatchTable::getBatchJSONLength()
    {
        return mBatchJSON.length();
    }

    size_t FeatureBatchTable::getBatchBodyLength()
    {
        return mBatchBodyLength;
    }

    void FeatureBatchTable::writeFeatureBody(String key, osg::Array* data)
    {
        if (data && data->getNumElements() == mFeatureLength)
        {
            uint32 offset = getFeatureBodyLength();
            GLenum component = data->getDataType();
            uint32 size = data->getTotalDataSize();

            Json::Value byteOffsetValue(Json::objectValue);
            byteOffsetValue["byteOffset"] = mFeatureBodyLength;
            byteOffsetValue["componentType"] = component == GL_BYTE ? "BYTE" :
                component == GL_UNSIGNED_BYTE ? "UNSIGNED_BYTE" :
                component == GL_SHORT ? "SHORT" :
                component == GL_INT ? "INT" :
                component == GL_UNSIGNED_INT ? "UNSIGNED_INT" :
                component == GL_FLOAT ? "FLOAT" : "DOUBLE";
            mFeagtureJSONValue[key] = byteOffsetValue;
            mFeatureBody.write((const char*)(data->getDataPointer()), size);
            mFeatureBodyLength += size;
        }
    }

    void FeatureBatchTable::writeBatchBody(String key, osg::Array* data)
    {
        if (data->getNumElements() == mBatchLength)
        {
            GLint dataSize = data->getDataSize();
            GLenum component = data->getDataType();
            uint32 size = data->getTotalDataSize();

            uint32 offset = getFeatureBodyLength();
            Json::Value byteOffsetValue(Json::objectValue);
            byteOffsetValue["byteOffset"] = mBatchBodyLength;
            byteOffsetValue["componentType"] = component == GL_BYTE ? "BYTE" : 
                component == GL_UNSIGNED_BYTE ? "UNSIGNED_BYTE" :
                component == GL_SHORT ? "SHORT" :
                component == GL_INT ? "INT" :
                component == GL_UNSIGNED_INT ? "UNSIGNED_INT" :
                component == GL_FLOAT ? "FLOAT" : "DOUBLE";

            byteOffsetValue["type"] = dataSize == 1 ? "SCALAR" :
                dataSize == 2 ? "VEC2 " : 
                dataSize == 3 ? "VEC3 " : "VEC4 ";
            mBatchJSONValue[key] = byteOffsetValue;
            mBatchBody.write((const char*)(data->getDataPointer()), size);
            mBatchBodyLength += size;
        }
        else
        {
            for (auto& i : mAttributeIndex)
            {
                if (i.second == key)
                {
                    i.second = "";
                }
            }
        }
    }

    size_t FeatureBatchTable::getFeatureJSONLength()
    {
        return mFeatureJSON.length();
    }

    size_t FeatureBatchTable::getFeatureBodyLength()
    {
        return mFeatureBodyLength;
    }
    
    StringStream& FeatureBatchTable::featureBody()
    {
        return mFeatureBody;
    }

    StringStream& FeatureBatchTable::batchBody()
    {
        return mBatchBody;
    }

    void FeatureBatchTable::pushAttribute(String key, String value)
    {
        if (mBatchStringAttribute.find(key) == mBatchStringAttribute.end())
        {
            mBatchStringAttribute[key] = Json::Value(Json::arrayValue);
            mAttributeIndex[mAttributeIndex.size()] = key;
        }
        mBatchStringAttribute[key].append(value);
    }


    void FeatureBatchTable::pushAttribute(String key, uint8 value)
    {
        if (!mFloatAttribute[key].valid())
        {
            mUByteAttribute[key] = new osg::UByteArray;
            mAttributeIndex[mAttributeIndex.size()] = key;
        }

        mUByteAttribute[key]->push_back(value);
    }

    void FeatureBatchTable::pushAttribute(String key, uint16 value)
    {
        if (!mUShortAttribute[key].valid())
        {
            mUShortAttribute[key] = new osg::UShortArray;
            mAttributeIndex[mAttributeIndex.size()] = key;
        }

        mUShortAttribute[key]->push_back(value);
    }


    void FeatureBatchTable::pushAttribute(String key, float value)
    {
        if (!mFloatAttribute[key].valid())
        {
            mFloatAttribute[key] = new osg::FloatArray;
            mAttributeIndex[mAttributeIndex.size()] = key;
        }

        mFloatAttribute[key]->push_back(value);
    }

    void FeatureBatchTable::pushAttribute(String key, uint32 value)
    {
        if (!mUIntAttribute[key].valid())
        {
            mUIntAttribute[key] = new osg::UIntArray;
            mAttributeIndex[mAttributeIndex.size()] = key;
        }

        mUIntAttribute[key]->push_back(value);
    }

    void FeatureBatchTable::pushAttribute(String key, int value)
    {
        if (!mIntAttribute[key].valid())
        {
            mIntAttribute[key] = new osg::IntArray;
            mAttributeIndex[mAttributeIndex.size()] = key;
        }

        mIntAttribute[key]->push_back(value);
    }

    void FeatureBatchTable::copyAttributes(FeatureBatchTable* rhs, std::map<uint32, uint32> batchIdMap)
    {
        setBatchLength(batchIdMap.size());
        for (auto& pair : rhs->mBatchStringAttribute)
        {
            Json::Value values = pair.second;
            if ((uint32)values.size() == rhs->mBatchLength)
            {
                String key = pair.first;

                for (auto id : batchIdMap)
                {
                    pushAttribute(key, values[id.second].asString());
                }
            }
        }

        for (auto i : rhs->mFloatAttribute)
        {
            for (auto id : batchIdMap)
            {
                pushAttribute(i.first, i.second->at(id.second));
            }
        }
        for (auto i : rhs->mIntAttribute)
        {
            for (auto id : batchIdMap)
            {
                pushAttribute(i.first, i.second->at(id.second));
            }
        }
        for (auto i : rhs->mUIntAttribute)
        {
            for (auto id : batchIdMap)
            {
                pushAttribute(i.first, i.second->at(id.second));
            }
        }
    }

    void FeatureBatchTable::writeToStream(std::ostream& output)
    {
        if (mExternalBytesLength == 0)
        {
            return;
        }

        if (mType == FBT_B3DM)
        {
            b3dmheader header;
            header.magic[0] = 'b', header.magic[1] = '3', header.magic[2] = 'd', header.magic[3] = 'm';
            header.version = 1;
            header.featureTableJSONByteLength = mFeatureJSON.length();
            header.featureTableBinaryByteLength = mFeatureBodyLength;
            header.batchTableJSONByteLength = mBatchJSON.length();
            header.batchTableBinaryByteLength = mBatchBodyLength;
            header.byteLength =
                sizeof(b3dmheader) +
                header.featureTableJSONByteLength +
                header.featureTableBinaryByteLength +
                header.batchTableJSONByteLength +
                header.batchTableBinaryByteLength + mExternalBytesLength;
            output.write((const char*)&header, sizeof(b3dmheader));
            output.write(mFeatureJSON.c_str(), header.featureTableJSONByteLength);
            output.write(mFeatureBody.str().c_str(), header.featureTableBinaryByteLength);
            output.write(mBatchJSON.c_str(), header.batchTableJSONByteLength);
            output.write(mBatchBody.str().c_str(), header.batchTableBinaryByteLength);
        }
        else if (mType == FBT_I3DM)
        {
            i3dmheader header;
            header.magic[0] = 'i', header.magic[1] = '3', header.magic[2] = 'd', header.magic[3] = 'm';
            header.version = 1;
            header.featureTableJSONByteLength = mFeatureJSON.length();
            header.featureTableBinaryByteLength = mFeatureBodyLength;
            header.batchTableJSONByteLength = mBatchJSON.length();
            header.batchTableBinaryByteLength = 0;
            header.byteLength =
                sizeof(i3dmheader) +
                header.featureTableJSONByteLength +
                header.featureTableBinaryByteLength +
                header.batchTableJSONByteLength +
                header.batchTableBinaryByteLength + mExternalBytesLength;

            header.gltfFormat = mGltfFormat;

            output.write((const char*)&header, sizeof(i3dmheader));
            output.write(mFeatureJSON.c_str(), header.featureTableJSONByteLength);
            output.write(mFeatureBody.str().c_str(), header.featureTableBinaryByteLength);
            output.write(mBatchJSON.c_str(), header.batchTableJSONByteLength);
        }
        else if (mType == FBT_PNTS)
        {
            b3dmheader header;
            header.magic[0] = 'p', header.magic[1] = 'n', header.magic[2] = 't', header.magic[3] = 's';
            header.version = 1;
            header.featureTableJSONByteLength = mFeatureJSON.length();
            header.featureTableBinaryByteLength = mFeatureBodyLength;
            header.batchTableJSONByteLength = mBatchJSON.length();
            header.batchTableBinaryByteLength = mBatchBodyLength;
            header.byteLength =
                sizeof(b3dmheader) +
                header.featureTableJSONByteLength +
                header.featureTableBinaryByteLength +
                header.batchTableJSONByteLength +
                header.batchTableBinaryByteLength + mExternalBytesLength;

            output.write((const char*)&header, sizeof(b3dmheader));
            output.write(mFeatureJSON.c_str(), header.featureTableJSONByteLength);
            output.write(mFeatureBody.str().c_str(), header.featureTableBinaryByteLength);
            output.write(mBatchJSON.c_str(), header.batchTableJSONByteLength);
            output.write(mBatchBody.str().c_str(), header.batchTableBinaryByteLength);
        }
    }

    uint32 FeatureBatchTable::bytesLength()
    {
        uint32 length = 
            mFeatureJSON.length()+
            mFeatureBodyLength +
            mBatchJSON.length()+
            mBatchBodyLength;

        if (mType == FBT_I3DM)
        {
            length += sizeof(i3dmheader);
        }
        else
        {
            length += sizeof(b3dmheader);
        }

        return length;
    }

}}//namespace
