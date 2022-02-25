#include "BatchGroup.h"
#include "OCUtility/StringConverter.h"
#include "TextureOptimize.h"

namespace OC
{
    namespace Cesium
    {
        BatchGroup::BatchGroup(FeatureBatchTable* featureBatchTable)
            :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
            , mFeatureBatchTable(featureBatchTable)
        {
        }

        BatchGroup::~BatchGroup()
        {

        }

        void BatchGroup::apply(osg::Geode& node)
        {
            geodeCount++;
            String refName = StringUtil::BLANK;
            osg::Matrix matrix = osg::Matrix::identity();

            osg::ref_ptr<osg::StateSet> pStateSet;

            osg::NodePath path = getNodePath();
            for (auto i : path)
            {
                if (osg::StateSet* stateset = i->getStateSet())
                {
                    if (!pStateSet.valid())
                    {
                        pStateSet = (osg::StateSet*)stateset->clone(osg::CopyOp::SHALLOW_COPY);
                    }

                    pStateSet->merge(*stateset);
                }

                if (osg::Transform* ts = i->asTransform())
                {
                    if (osg::MatrixTransform* mt = ts->asMatrixTransform())
                    {
                        matrix.postMult(mt->getMatrix());
                    }
                    else
                    {
                        return;
                    }
                }

                String name = i->getName();
                if (!name.empty())
                {
                    refName = name;
                }
            }

            assignBatch(&node, matrix, pStateSet, refName);
        }

        void BatchGroup::assignBatch(osg::Geode* geode,osg::Matrix matrix,osg::StateSet* stateset,String referenceName)
        {
            long batchId = -1;
            
            osg::ref_ptr<osg::StateSet> pStateSet = stateset;

            if (!pStateSet.valid())
            {
                pStateSet = geode->getStateSet();
            }
            else if (geode->getStateSet())
            {
                pStateSet->merge(*geode->getStateSet());
            }

            for (long j = 0; j < (long)geode->getNumDrawables(); j++)
            {
                if (osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(j)))
                {
                    if ((mFeatureBatchTable.valid() && mFeatureBatchTable->type() == FBT_B3DM))
                    {
                        String name = geode->getName();
                        if(name.empty())
                        {
                            name = geom->getName();
                            if (name.empty())
                            {
                                name = referenceName;
                            }
                        } 
                        if (mNameBatchId.find(name) == mNameBatchId.end())
                        {
                            batchId = mFeatureBatchTable->getBatchLength();

                            if (FeatureAttributeCallback* callback = mFeatureBatchTable->getAttributeCallback())
                            {
                                callback->onSetAttirubte(name, mFeatureBatchTable);
                            }
                            else
                            {
                                mFeatureBatchTable->pushAttribute("id", StringUtil::GenGUID().substr(0, 8));
                                mFeatureBatchTable->pushAttribute("name", name);
                            }
                            mFeatureBatchTable->setFeatureLength(batchId + 1);
                            mFeatureBatchTable->setBatchLength(batchId + 1);
                            mNameBatchId[name] = batchId;
                        }
                        else
                        {
                            batchId = mNameBatchId[name];
                        }
                    }

                    osg::ref_ptr<osg::StateSet> childStateSet = geom->getStateSet();
                    if (pStateSet.valid())
                    {
                        if (childStateSet.valid())
                        {
                            pStateSet = (osg::StateSet*)pStateSet->clone(osg::CopyOp::SHALLOW_COPY);
                            pStateSet->merge(*childStateSet);
                        }
                    }
                    else
                    {
                        pStateSet = childStateSet;
                    }

                    osg::ref_ptr<gltfMaterial> mat = new gltfMaterial(pStateSet.get());

                    if (geom->getNumPrimitiveSets() > 0)
                    {
                        osg::PrimitiveSet* pset = geom->getPrimitiveSet(0);
                        GLenum mode = pset->getMode();
                        if (mode == osg::PrimitiveSet::LINES
                        || mode == osg::PrimitiveSet::LINE_STRIP
                        || mode == osg::PrimitiveSet::LINE_LOOP)
                        {
                            mat = new gltfMaterial(NULL);
                            mat->mode = osg::PrimitiveSet::LINES;
                        }

                        String key = mat->key();
                        if (!mBatchGeodeMap[key].valid())
                        {
                            BatchGeode* batchgeode = new BatchGeode(mat);
                            mBatchGeodeList.push_back(batchgeode);
                            mBatchGeodeMap[key] = batchgeode;
                            if (mat->image)
                            {
                                TextureOptimize::scaleDimansion(mat->image);
                            }
                        }

                        mBatchGeodeMap[key]->merge(geom, batchId, matrix);
#if 1
                        if (mat->image)
                        {
                            String key = mat->image->getFileName();
                            key += (mat->wrap_s == (int)GL_REPEAT ? "_REPEAT" : "_CLAMP");
                            key += (mat->wrap_t == (int)GL_REPEAT ? "_REPEAT" : "_CLAMP");
                            mImageUseCount[key]++;
                        }
#endif
                    }
                }
            }
        }

        void BatchGroup::convert(osg::Node* node)
        {
            if (node->asGeode())
            {
                apply(*node->asGeode());
            }
            else
            {
                traverse(*node);
            }

            std::cout << "Geode:(" << geodeCount;
            if (mFeatureBatchTable.valid())
            {
                std::cout << "),Feature:(" << mFeatureBatchTable->getFeatureLength();
            }
            std::cout << "),Batch:(" << mBatchGeodeList.size() << ")";

            if (Cesium::gltfConfig::mergeClampTexture)
            {
                TextureOptimize::merge_clamp(mBatchGeodeList, mBatchGeodeMap);
            }
            if (Cesium::gltfConfig::mergeRepeatTexture)
            {
                TextureOptimize::merge_repeat(mBatchGeodeList, mBatchGeodeMap);
            }

            std::cout << std::endl;

            submitImage();
        }

        void BatchGroup::submitImage()
        {
            for (auto i : mBatchGeodeList)
            {
                BatchGeode* batchGeode = i.get();
                gltfMaterial* mat = batchGeode->material();
                if (mat->image)
                {
                    String file = mat->image->getFileName();
                    if (Cesium::gltfConfig::enableWebpCompress)
                    {
                        String ext = osgDB::getFileExtension(file);
                        String webpfile = StringUtil::replaceAll(file, "." + ext, ".webp");
                        mat->image->setFileName(webpfile);
                    }
                    else if (Cesium::gltfConfig::enableKtxCompress)
                    {
                        String ext = osgDB::getFileExtension(file);
                        String ktxfile = StringUtil::replaceAll(file, "." + ext, ".ktx2");
                        mat->image->setFileName(ktxfile);
                    }

                    mat->imageData = _gltfHelper.getOrCreateImageData(mat->image);
                    if (!mat->imageData.valid() && Cesium::gltfConfig::enableWebpCompress)
                    {
                        mat->image->setFileName(file);
                        mat->imageData = _gltfHelper.getOrCreateImageData(mat->image);
                    }
                }

                //
                boundingBox().expandBy(batchGeode->boundingBox());
            }
        }

        bool BatchGroup::submitBatch()
        {
            BatchGeodeList& list = getBatchGeodeList();
            for (auto i : list)
            {
                i->commit(&_gltfHelper);
            }
            bool ret =  !list.empty();
            list.clear();
            return ret;
        }

    }
}