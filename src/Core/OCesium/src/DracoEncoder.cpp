#include "DracoEncoder.h"
#include "BatchGeode.h"
#include "draco/compression/encode.h"
#include "draco/compression/decode.h"

namespace OC{namespace Cesium
{
    DracoEncoder::DracoEncoder()
    {
        buffer = new draco::EncoderBuffer;
    }

    DracoEncoder::~DracoEncoder()
    {
        delete buffer;
        buffer = NULL;
    }

    const char* DracoEncoder::data()
    {
        return buffer->data();
    }

    size_t DracoEncoder::size()
    {
        return buffer->size();
    }

    bool DracoEncoder::encode(BatchGeode* batchGeode)
    {
        int quantization_bits = Cesium::gltfConfig::quantizationbits;
        bool use_identity_mapping = true;
        std::unique_ptr<draco::Mesh> pc = std::unique_ptr<draco::Mesh>(new draco::Mesh());

        osg::Vec4Array* textureTransform = batchGeode->textureTransform();
        osg::Vec3Array* positions = batchGeode->positions();
        osg::Vec3Array* normals = batchGeode->normals();
        osg::Vec3Array* colors = batchGeode->colors();
        osg::Vec2Array* textures = batchGeode->textures();
        osg::FloatArray* batchids = batchGeode->batchIds();

        uint32 num_positions_ = positions->size();

        pc->set_num_points(num_positions_);

        {
            draco::GeometryAttribute va;
            va.Init(draco::GeometryAttribute::POSITION, nullptr, 3, draco::DT_FLOAT32, false,
                sizeof(float) * 3, 0);
            int attrId = pc->AddAttribute(va, use_identity_mapping, num_positions_);
            mAttributeMap["POSITION"] = attrId;
            pc->attribute(attrId)->buffer()->Write(0, positions->getDataPointer(), positions->getTotalDataSize());
        }

        if (normals != NULL)
        {
            draco::GeometryAttribute va;
            va.Init(draco::GeometryAttribute::NORMAL, nullptr, 3, draco::DT_FLOAT32, false,
                sizeof(float) * 3, 0);
            int attrId = pc->AddAttribute(va, use_identity_mapping, num_positions_);
            mAttributeMap["NORMAL"] = attrId;
            pc->attribute(attrId)->buffer()->Write(0, normals->getDataPointer(), normals->getTotalDataSize());
        }

        if (colors != NULL)
        {
            draco::GeometryAttribute va;
            va.Init(draco::GeometryAttribute::COLOR, nullptr, 3, draco::DT_FLOAT32, false,
                sizeof(float) * 3, 0);
            int attrId = pc->AddAttribute(va, use_identity_mapping, num_positions_);
            mAttributeMap["COLOR_0"] = attrId;
            pc->attribute(attrId)->buffer()->Write(0, colors->getDataPointer(), colors->getTotalDataSize());
        }

        if (textures != NULL)
        {
            if (textureTransform != NULL)
            {
                draco::GeometryAttribute va;
                va.Init(draco::GeometryAttribute::TEX_COORD, nullptr, 4, draco::DT_FLOAT32, false,
                    sizeof(float) * 4, 0);
                int attrId = pc->AddAttribute(va, use_identity_mapping, num_positions_);
                mAttributeMap["_TEXCOORD_TRANSFORM"] = attrId;
                pc->attribute(attrId)->buffer()->Write(0, textureTransform->getDataPointer(), textureTransform->getTotalDataSize());
            }

            if (textureTransform == NULL || !batchGeode->mHasErrorTexCoord/*¾«¶ÈÔÊÐí*/)
            {
                draco::GeometryAttribute va;
                va.Init(draco::GeometryAttribute::TEX_COORD, nullptr, 2, draco::DT_FLOAT32, false,
                    sizeof(float) * 2, 0);
                int attrId = pc->AddAttribute(va, use_identity_mapping, num_positions_);
                mAttributeMap["TEXCOORD_0"] = attrId;
                pc->attribute(attrId)->buffer()->Write(0, textures->getDataPointer(), textures->getTotalDataSize());
            }
            else
            {
                osg::ref_ptr<osg::Vec2dArray> tarray = new osg::Vec2dArray;
                tarray->insert(tarray->end(), textures->begin(), textures->end());
                draco::GeometryAttribute va;
                va.Init(draco::GeometryAttribute::TEX_COORD, nullptr, 2, draco::DT_FLOAT64, false,
                    sizeof(double) * 2, 0);
                int attrId = pc->AddAttribute(va, use_identity_mapping, num_positions_);
                mAttributeMap["TEXCOORD_0"] = attrId;
                pc->attribute(attrId)->buffer()->Write(0, tarray->getDataPointer(), tarray->getTotalDataSize());
            }
        }

        int quantization_bitsGeneric = quantization_bits;
        if (batchids != NULL)
        {
            draco::GeometryAttribute va;
            va.Init(draco::GeometryAttribute::GENERIC, nullptr, 1, draco::DT_FLOAT32, false,
                sizeof(float), 0);
            int attrId = pc->AddAttribute(va, use_identity_mapping, num_positions_);
            mAttributeMap["_BATCHID"] = attrId;
            pc->attribute(attrId)->buffer()->Write(0, batchids->getDataPointer(), batchids->getTotalDataSize());
            uint32 maxBatchId = batchGeode->getContainBatchIdsMap().rbegin()->first;
            int i = quantization_bits / 2;
            while (pow(2,i) < maxBatchId)
            {
                i++;
            }
            quantization_bitsGeneric = i;
        }

        if (osg::DrawElements* drawElements = dynamic_cast<osg::DrawElements*>(batchGeode->primitive()))
        {
            if (drawElements->getMode() == osg::PrimitiveSet::LINES)
            {
                size_t n = drawElements->getNumIndices() / 2;
                for (size_t i = 0; i < n; i++)
                {
                    draco::Mesh::Face f;
                    f[0] = drawElements->getElement(i * 2);
                    f[1] = drawElements->getElement(i * 2 + 1);
                    f[2] = drawElements->getElement(i * 2 + 1);
                    pc->AddFace(f);
                }
            }
            else
            {
                size_t n = drawElements->getNumIndices() / 3;
                for (size_t i = 0; i < n; i++)
                {
                    draco::Mesh::Face f;
                    f[0] = drawElements->getElement(i * 3);
                    f[1] = drawElements->getElement(i * 3 + 1);
                    f[2] = drawElements->getElement(i * 3 + 2);
                    pc->AddFace(f);
                }
            }

        }

        draco::Encoder encoder;
        encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION,
            quantization_bits);
        encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL,
            quantization_bits);
        encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD,
            quantization_bits - 4);
        encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC,
            quantization_bits);
        encoder.SetSpeedOptions(5, 5);
        const draco::Status status = encoder.EncodeMeshToBuffer(*pc, buffer);

        return status.ok();
    }
}}//namespace