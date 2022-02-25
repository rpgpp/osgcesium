#include "B3dmWriter.h"
#include "GLTFObject.h"

namespace OC
{namespace Cesium {

    B3dmWriter::B3dmWriter()
        :mFeatureBatchTable(new FeatureBatchTable(FBT_B3DM))
    {
        mGltfObject= new GLTFObject(mFeatureBatchTable);
    }

    B3dmWriter::~B3dmWriter()
    {
    }

    bool B3dmWriter::writeToFile(String filename)
    {
        std::fstream fout(filename.c_str(), std::ios::out | std::ios::binary);
        if (!fout.is_open())
            return false;

        writeToStream(fout);

        fout.close();

        return true;
    }

    bool B3dmWriter::writeToStream(std::ostream& output)
    {
        uint32 pos_beg = output.tellp();

        getFeatureBatchTable()->format();

        //write gltf body
        uint32 headerBytesLength = getFeatureBatchTable()->bytesLength();
        output.seekp(pos_beg + headerBytesLength);

        mGltfObject->writeToStream(output,true);

        uint32 gltfBytesLength = mGltfObject->bytesLength();

        if (gltfBytesLength == 0)
        {
            return false;
        }

        uint32 gltfDataPadding = 4 - (gltfBytesLength % 4);
        if (gltfDataPadding == 4) gltfDataPadding = 0;
        output.write("\0\0\0", gltfDataPadding);
        gltfBytesLength += gltfDataPadding;

        //write header
        output.seekp(pos_beg);
        getFeatureBatchTable()->setExternalBytesLength(gltfBytesLength);
        getFeatureBatchTable()->writeToStream(output);

        output.seekp(0, std::ios::end);

        mBytesLength = headerBytesLength + gltfBytesLength;

        return mBytesLength != 0;
    }

    uint32 B3dmWriter::bytesLength()
    {
        return mGltfObject->bytesLength();
    }

    GLTFObject* B3dmWriter::getGltfObject()
    {
        return mGltfObject.get();
    }

    FeatureBatchTable* Cesium::B3dmWriter::getFeatureBatchTable()
    {
        return mFeatureBatchTable.get();
    }

    void B3dmWriter::copyAttributes(B3dmWriter* rhs)
    {
        std::map<uint32, uint32> containBatchIdsMap;

        BatchGeodeList mBatchGeodeList = rhs->mGltfObject->getBatchGeodeList();
        for (auto i : mBatchGeodeList)
        {
            std::map<uint32, uint32> ids = i->getContainBatchIdsMap();
            for (auto j : ids)
            {
                if (containBatchIdsMap.find(j.first) == containBatchIdsMap.end())
                {
                    containBatchIdsMap[j.first] = containBatchIdsMap.size();
                }
            }
            i->resetBatchId(containBatchIdsMap);
        }

        getFeatureBatchTable()->copyAttributes(rhs->getFeatureBatchTable(), containBatchIdsMap);
    }

    struct {
        bool operator()(osg::ref_ptr<BatchGeode> a, osg::ref_ptr<BatchGeode> b) const
        {
            return a->getCapacity() < b->getCapacity();
        }
    } customLess;

    std::vector<osg::ref_ptr<B3dmWriter> > B3dmWriter::splitByCapcity()
    {
        std::vector<osg::ref_ptr<B3dmWriter> > list;

        size_t splitSize = gltfConfig::sectionSize * 1048576;
        BatchGeodeList geodeList = getGltfObject()->getBatchGeodeList();
        std::sort(geodeList.begin(), geodeList.end(), customLess);
        while (!geodeList.empty())
        {
            osg::ref_ptr<B3dmWriter> childB3dm = new B3dmWriter();
            if (splitSize == 0)
            {
                geodeList.clear();
            }
            else
            {
                size_t size = 0;
                osg::ref_ptr<BatchGeode> batchGeode = geodeList.back();
                childB3dm->getGltfObject()->getBatchGeodeList().push_back(batchGeode);
                geodeList.pop_back();
                size += batchGeode->getCapacity();
                while (size < splitSize && !geodeList.empty())
                {
                    osg::ref_ptr<BatchGeode> batchGeode = geodeList.front();
                    size += batchGeode->getCapacity();
                    geodeList.erase(geodeList.begin());
                    childB3dm->getGltfObject()->getBatchGeodeList().push_back(batchGeode);
                }
            }

            if (list.empty() && geodeList.empty())
            {
                list.push_back(this);
            }
            else
            {
                list.push_back(childB3dm);
                childB3dm->copyAttributes(this);
            }
        }

        return list;
    }

}}
