#include "BatchGeode.h"
#include "gltfMaterial.h"
#include "GltfHelper.h"

namespace OC
{
    namespace Cesium
    {
        BatchGeode::BatchGeode(gltfMaterial* material)
            :mMaterial(material)
        {
            mPositions = new osg::Vec3Array;
            GLenum mode = material->mode;

            if (mHasTexture = material->image && mode == osg::PrimitiveSet::TRIANGLES)
            {
                mTextureCoord = new osg::Vec2Array;
            }

            mNormals = new osg::Vec3Array;
            mColors = new osg::Vec3Array;
            mGltfTrifunc.set = new osg::DrawElementsUInt(mode);
        }

        BatchGeode::~BatchGeode()
        {
        }

        osg::Vec4Array* BatchGeode::textureTransform()
        {
            return mTextureTransform.get();
        }

        osg::Vec3Array* BatchGeode::positions()
        {
            return mPositions.get();
        }

        osg::Vec3Array* BatchGeode::normals()
        {
            return mNormals.get();
        }

        osg::Vec3Array* BatchGeode::colors()
        {
            return mColors.get();
        }

        osg::Vec2Array* BatchGeode::textures()
        {
            return mTextureCoord.get();
        }

        osg::FloatArray* BatchGeode::batchIds()
        {
            return mBatchIdArray.get();
        }

        osg::PrimitiveSet* BatchGeode::primitive()
        {
            return mGltfTrifunc.set;
        }

        gltfMaterial* BatchGeode::material()
        {
            return mMaterial.get();
        }

        void BatchGeode::dracoEncode()
        {
            if (mDracoEncoder.size() == 0)
            {
                if (!mHasNormals)
                {
                    mNormals = NULL;
                }
                mDracoEncoder.encode(this);
            }
        }

        osg::Geometry* BatchGeode::toGeometry()
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            geometry->setVertexArray(positions());
            geometry->addPrimitiveSet(primitive());
            return geometry.release();
        }

        void BatchGeode::commit(GltfHelper* _gltfHelper)
        {
            int materialsId = _gltfHelper->pushMaterial(material());

            osg::PrimitiveSet* primitiveSet = primitive();

            int primitiveId = _gltfHelper->pushPrimitive(primitiveSet, materialsId);

            //
            osg::Vec3Array* position = positions();
            _gltfHelper->pushVertex(primitiveId, position, boundingBox());

            if (mHasNormals)
            {
                _gltfHelper->pushNormal(primitiveId, normals());
            }

            if (mHasColors)
            {
                _gltfHelper->pushColor(primitiveId, colors());

            }
            if (mHasTexture)
            {
                _gltfHelper->pushTexture(primitiveId, textures());
                if (mHasTextureTransform)
                {
                    _gltfHelper->pushAttribute("_TEXCOORD_TRANSFORM", textureTransform());
                }
            }

            if (mHasBatchId)
            {
                _gltfHelper->pushBatchId(primitiveId, batchIds());
            }

            if (Cesium::gltfConfig::enableDracoCompress)
            {
                dracoEncode();
                _gltfHelper->setPrimitiveDraco(primitiveId,
                    getDracoEncoder().mAttributeMap,
                    getDracoEncoder().data(),
                    getDracoEncoder().size());
            }
        }

        void BatchGeode::merge(osg::Geometry* geometry, long batchID, osg::Matrix matrix)
        {
            osg::Vec3Array* vertex = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
            if (vertex == NULL)
            {
                return;
            }

            if (geometry->getNormalArray())
            {
                mHasNormals = !gltfConfig::disableLight;
            }

            if (batchID > -1)
            {
                if (!mBatchIdArray.valid())
                {
                    mBatchIdArray = new osg::FloatArray;
                    mHasBatchId = true;
                }
            }

            pushBatchId(batchID);

            for (unsigned k = 0; k < geometry->getNumPrimitiveSets(); ++k)
            {
                osg::PrimitiveSet* pset = geometry->getPrimitiveSet(k);
                GLenum mode = pset->getMode();

                switch (mode)
                {
                case(osg::PrimitiveSet::TRIANGLES):
                case(osg::PrimitiveSet::TRIANGLE_STRIP):
                case(osg::PrimitiveSet::TRIANGLE_FAN):
                case(osg::PrimitiveSet::QUADS):
                case(osg::PrimitiveSet::QUAD_STRIP):
                case(osg::PrimitiveSet::POLYGON):
                    pset->accept(mGltfTrifunc);
                    break;
                case(osg::PrimitiveSet::LINES):
                case(osg::PrimitiveSet::LINE_STRIP):
                case(osg::PrimitiveSet::LINE_LOOP):
                    pset->accept(mGltfTrifunc);
                    break;
                default:
                    break;
                }
            }

            osg::Vec3Array* normal = dynamic_cast<osg::Vec3Array*>(geometry->getNormalArray());
            osg::Vec3Array* color = dynamic_cast<osg::Vec3Array*>(geometry->getColorArray());
            mHasColors = (color != NULL);
            osg::Vec2Array* texCoords = NULL;
            osg::Vec3Array* texCoords3 = NULL;
            if(geometry->getTexCoordArrayList().size() > 0)
            {
                texCoords = dynamic_cast<osg::Vec2Array*>(geometry->getTexCoordArray(0));
                texCoords3 = dynamic_cast<osg::Vec3Array*>(geometry->getTexCoordArray(0));
            }

            size_t size = vertex->size();

           for (unsigned int i = 0; i < size; ++i)
           {
                const osg::Vec3f& v = (*vertex)[i];

                osg::Vec3f pos = v * matrix;
                positions()->push_back(pos);
                mBoundingBox.expandBy(pos);
                
                //normal every geometry,check when write to gltf
                {
                    osg::Vec3 n = osg::Z_AXIS;
                    if (normal)
                    {
                        osg::Vec3 n1 = normal->at(i)/* * matrix*/;
                        if (n1.length() > 0.0f)
                        {
                            n1.normalize();
                            n = n1;
                        }
                    }

                    normals()->push_back(n);
                }

                {
                    osg::Vec3 c = osg::Vec3(1.0,1.0,1.0);
                    if (color)
                    {
                        c = color->at(i);
                    }
                    colors()->push_back(c);
                }

                if (mHasTexture)
                {
                    float tol = Cesium::gltfConfig::TextrueCoordError;
                    osg::Vec2f t = texCoords ? texCoords->at(i) : 
                        texCoords3 ? osg::Vec2(texCoords3->at(i).x(), texCoords3->at(i).y()) : osg::Vec2(0.0, 0.0);

                    if (osg::isNaN(t.x()) || osg::isNaN(t.y()))
                    {
                        t = osg::Vec2(0.0, 0.0);
                    }

                    if ((t.x() > tol || t.y() > tol)
                        || (t.x() < -tol || t.y() < -tol))
                    {
                        mHasErrorTexCoord = true;
                    }

                    textures()->push_back(t);

                    if (t.x() < -1e-3 || t.x() > 1.001
                    || t.y() < -1e-3 || t.y() > 1.001)
                    {
                        mWrapSTOutofRange = true;
                    }
                }

                if (mHasBatchId)
                {
                    mBatchIdArray->push_back(batchID);
                }
            }

            mGltfTrifunc.offset += size;
        }

        void BatchGeode::merge(BatchGeode* bgeode)
        {
            uint32 offset = mPositions->size();
            for (auto i : *bgeode->mGltfTrifunc.set)
            {
                mGltfTrifunc.set->push_back(i + offset);
            }

            mBoundingBox.expandBy(bgeode->mBoundingBox);
            mPositions->insert(mPositions->end()
                , bgeode->mPositions->begin()
                , bgeode->mPositions->end());
            mNormals->insert(mNormals->end()
                , bgeode->mNormals->begin()
                , bgeode->mNormals->end());

            if (mHasTexture)
            {
                mTextureCoord->insert(mTextureCoord->end()
                    , bgeode->mTextureCoord->begin()
                    , bgeode->mTextureCoord->end());
            }

            if (mHasBatchId)
            {
                mBatchIdArray->insert(mBatchIdArray->end()
                    , bgeode->mBatchIdArray->begin()
                    , bgeode->mBatchIdArray->end());

                for (auto i : bgeode->mContainBatchIdsMap)
                {
                    pushBatchId(i.first);
                }
            }

            if (mHasTextureTransform)
            {
                mTextureTransform->insert(mTextureTransform->end()
                    , bgeode->mTextureTransform->begin()
                    , bgeode->mTextureTransform->end());
            }

            mHasErrorTexCoord = mHasErrorTexCoord || bgeode->mHasErrorTexCoord;
        }

        void BatchGeode::transTexCoord(osg::Matrix matrix)
        {
            if (mHasTexture)
            {
                for (auto& i : *mTextureCoord)
                {
                    osg::Vec2 tc = i;
                    i.set(tc[0] * matrix(0, 0) + tc[1] * matrix(1, 0) + matrix(3, 0),
                        tc[0] * matrix(0, 1) + tc[1] * matrix(1, 1) + matrix(3, 1));
                }
            }
        }

        void BatchGeode::setTextureTransform(osg::Vec2 offset, osg::Vec2 scale)
        {
            mHasTextureTransform = true;
            size_t size = mPositions->size();
            mTextureTransform = new osg::Vec4Array;
            osg::Vec4 trans(offset.x(), offset.y(), scale.x(), scale.y());
            mTextureTransform->insert(mTextureTransform->end(), size, trans);
        }

        size_t BatchGeode::getCapacity()
        {
            if (mBytesLength > 0)
            {
                return mBytesLength;
            }

            mBytesLength += positions()->getTotalDataSize();
            mBytesLength += mGltfTrifunc.set->getTotalDataSize();

            if (mHasNormals)
            {
                mBytesLength += normals()->getTotalDataSize();
            }
            if (mHasTexture)
            {
                mBytesLength += textures()->getTotalDataSize();
            }
            if (mHasTextureTransform)
            {
                mBytesLength += textures()->getTotalDataSize();//double X2
                mBytesLength += textureTransform()->getTotalDataSize();
            }
            if (mHasBatchId)
            {
                mBytesLength += batchIds()->getTotalDataSize();
            }

            //draco
            if (Cesium::gltfConfig::enableDracoCompress)
            {
                mBytesLength *= 0.16;
            }

            if (mHasTexture && mMaterial->imageData)
            {
                mBytesLength += mMaterial->imageData->getTotalDataSize();
            }

            //base64 length
            mBytesLength = (mBytesLength * 1.333 + mBytesLength % 76);

            return mBytesLength;
        }

        void BatchGeode::resetBatchId(std::map<uint32, uint32> containBatchIdsMap)
        {
            if (mBatchIdArray.valid())
            {
                for (auto& i : *mBatchIdArray)
                {
                    uint32 t = i;
                    i = containBatchIdsMap[t];
                }
            }
        }

        void BatchGeode::pushBatchId(uint32 batchId)
        {
            if (mContainBatchIdsMap.find(batchId) == mContainBatchIdsMap.end())
            {
                mContainBatchIdsMap[batchId] = mContainBatchIdsMap.size();
            }
        }

        std::map<uint32, uint32> BatchGeode::getContainBatchIdsMap()
        {
            return mContainBatchIdsMap;
        }

    }
}
