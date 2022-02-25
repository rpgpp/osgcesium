#include "TilesetSaveStrategy.h"
#include "PipeTable.h"
#include "OCesium/CesiumTool.h"
#include "OCesium/CmptWriter.h"
#include "OCesium/I3dmWriter.h"

namespace OC
{
	namespace Modeling
	{
		using namespace Cesium;

		TilesetSaveStrategy::TilesetSaveStrategy()
		{
		}

		void TilesetSaveStrategy::writeTileset()
		{
			osg::BoundingBox box = Cesium::CesiumTool::rect2box(mRefTable->getMateRecord()->getExtent());
			mRootTileset->geometricError() = mRootTileset->root()->geometricError() = mRootBoundBox.radius() * 2.0 * gltfConfig::geometryErroRatio;
			mRootTileset->root()->boundingVolume()->region() = box;
			String tilesetfile = mWorkPath + "tileset.json";
			OC_WRITE_Tiltset(mRootTileset, tilesetfile);
		}

		TilesetSaveStrategy::~TilesetSaveStrategy()
		{
			writeTileset();
		}

		bool TilesetSaveStrategy::init()
		{
			mWorkPath = mWorkPath + mRefTable->getName() + "/";
			osgDB::makeDirectory(mWorkPath);
			mRootTileset = OC_NEW_Tiltset();
			return true;
		}

		TilesetSaveStrategy::GlobalTiles TilesetSaveStrategy::findGlobal(IGroupModelData* data)
		{
			osg::Matrix worldMatrix = mRefTable->getProjectTool()->computeTransform(data->getOffset());
			osg::Vec3 wolrdPosition = worldMatrix.getTrans();
			auto calc_rn = [](CVector3 position) {
				position.normalise();
				return Vector3ToVec3d(CVector3::UNIT_Z.crossProduct(position).normalisedCopy());
			};

			GlobalTiles i3dmMap;

			IModelDataList mList = data->getList();
			for (IModelData* d : mList)
			{
				if (d->isGlobalURL())
				{
					String mEnName = d->asPipeNode()->getInfo().getModelNode().getStringValue(ModelNode::NameEnName);
					
					if (!i3dmMap[mEnName].valid())
					{
						i3dmMap[mEnName] = new I3dmWriter;
					}
					I3dmWriter* i3dmWriter = i3dmMap[mEnName];

					CVector3 offset = d->asPipeNode()->getInfo().getModelPosition();
					CVector3 LonLatH = mRefTable->getProjectTool()->localTo4326(offset);

					i3dmWriter->setUrl("../../PipeInstances/" + mEnName + ".glb");
					osg::Matrix matrix = mRefTable->getProjectTool()->computeTransform(LonLatH);
					osg::Vec3 position = matrix.getTrans();

					osg::Vec3 normalUp = position;
					normalUp.normalize();

					osg::Vec3 normalRight = calc_rn(Vec3ToVector3(normalUp));

					{
						CVector3 direction = d->asPipeNode()->getInfo().getConstDirection();
						if (CVector3::ZERO == direction)
						{
							direction = d->asPipeNode()->getInfo().calcModelDirection();
						}

						if (d->asPipeNode()->getInfo().getModelNode().getBoolValue(ModelNode::NameMustHorizontal, true))
						{
							direction.z = 0;
						}

						direction.normalise();

						CVector3 faceDirection = d->asPipeNode()->getInfo().getModelNode().getDefineFaceDirection().second;
						double d = faceDirection.angleBetween(direction).valueRadians();
						
						osg::Quat quat;
						quat.makeRotate(d + osg::PI_2, normalUp);
						normalRight = quat * normalRight;
						normalRight.normalize();
					}

					i3dmWriter->push(matrix.getTrans() - wolrdPosition, normalUp, normalRight);

					CVector3 scale = d->getInnerRecord()->getScale();
					i3dmWriter->push_scale(osg::Vec3(scale.x, scale.z, scale.y));
					i3dmWriter->getFeatureBatchTable()->pushAttribute("id","instance");
					i3dmWriter->getFeatureBatchTable()->pushAttribute("name",
						d->getInnerRecord()->getTable() + "_" + StringConverter::toString(d->getInnerRecord()->getGID()));
				}
			}

			return i3dmMap;
		}

		inline CRectangle getTileRectangle(IModelTable* mRefTable, IGroupModelData* groupMD)
		{
			CMetaRecord* meta = mRefTable->getMateRecord();
			int size = Math::Pow(2, meta->getLevel() - 1);
			CRectangle extent = meta->getExtent();
			double w = extent.getWidth() / size;
			double h = extent.getHeight() / size;
			CRectangle tileExtent;
			tileExtent.getMinimum().x = extent.getMinimum().x + w * groupMD->getCol();
			tileExtent.getMaximum().x = tileExtent.getMinimum().x + w;
			tileExtent.getMinimum().y = extent.getMinimum().y + h * groupMD->getRow();
			tileExtent.getMaximum().y = tileExtent.getMinimum().y + h;
			return tileExtent;
		}

		bool TilesetSaveStrategy::writeFeature(FeatureRecord* record)
		{
			bool result = false;

			if (IGroupModelData* groupMD = dynamic_cast<IGroupModelData*>(record))
			{
				String tilekey = "X" + StringConverter::toString(groupMD->getRow()) + "_Y" + StringConverter::toString(groupMD->getCol());

				osg::ref_ptr<osg::Node> node = groupMD->getModelNode();
				if (node.valid())
				{
					osgUtil::SmoothingVisitor sv;
					node->accept(sv);

					if (!mTiltWriterMap[tilekey].valid())
					{
						mTiltWriterMap[tilekey] = new B3dmWriter;
					}

					B3dmWriter* b3dmWriter = mTiltWriterMap[tilekey];

					if (groupMD->mDataDistinct & IGroupModelData::GEOM)
					{
						b3dmWriter->getGltfObject()->convert(node);
						mRootBoundBox.expandBy(b3dmWriter->getGltfObject()->boundingBox());
					}

					if (StringUtil::endsWith(groupMD->getTable(), NodeTableSubfix, false))
					{
						String subdir = mWorkPath + tilekey + "/";
						osgDB::makeDirectory(mWorkPath + tilekey);

						GlobalTiles globalTiles = findGlobal(groupMD);

						String uri;
						if (globalTiles.size() > 0)
						{
							uri = tilekey + ".cmpt";
							osg::ref_ptr<CmptWriter> cmptWriter = new CmptWriter;
							cmptWriter->push(b3dmWriter);
							for (auto i : globalTiles)
							{
								cmptWriter->push(i.second);
							}
							cmptWriter->writeCmpt(subdir + "/" + uri);
						}
						else
						{
							uri = tilekey + ".b3dm";
							b3dmWriter->writeToFile(subdir + "/" + uri);
						}

						String url = tilekey + ".json";

						CRectangle tileExtent = getTileRectangle(mRefTable, groupMD);
						osg::Matrix worldMatrix = mRefTable->getProjectTool()->computeTransform(groupMD->getOffset());
						osg::Matrix transform = osg::Matrix::translate(worldMatrix.getTrans());
						osg::ref_ptr<TDTILES::Tileset> tileset = OC_NEW_Tiltset();
						tileset->root()->content()->uri() = uri;
						tileset->root()->boundingVolume()->region() = CesiumTool::rect2box(tileExtent);;
						tileset->geometricError() = tileset->root()->geometricError() = 0.0;
						OC_WRITE_Tiltset(tileset, subdir + url);

						//push to root
						tileset->root()->transform() = transform;
						tileset->geometricError() = tileset->root()->geometricError() = CesiumTool::computeGeometryError(tileExtent) * gltfConfig::geometryErroRatio;
						tileset->root()->content()->uri() = tilekey + "/" + url;
						mRootTileset->root()->children().push_back(tileset->root());
					}
				}

				result = true;
			}

			record->getData().setNull();

			return result;
		}
	}
}

