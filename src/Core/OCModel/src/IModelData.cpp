#include "IModelData.h"
#include "IModelTable.h"
#include "PipeNode.h"
#include "IModelFactory.h"
#include "OCLayer/IDatabaseObject.h"

namespace OC
{
	namespace Modeling
	{
		IModelData::IModelData():mState(STATE_NORMAL)
			,mOwnerTable(NULL)
			,mCenter(CVector3::ZERO)
			,mWorldZVector(CVector3::UNIT_Z)
		{
		}

		IModelData::~IModelData(void)
		{
		}

		void IModelData::setTemplateRecord(TemplateRecord* templaterecord)
		{
			mTemplateRecord = templaterecord;
		}

		TemplateRecord* IModelData::getTemplateRecord()
		{
			return mTemplateRecord;
		}

		CVector3 IModelData::getWorldZVector()
		{
			return mWorldZVector;
		}

		void IModelData::setWorldZVector(CVector3 direction)
		{
			mWorldZVector = direction;
		}

		IModelTable* IModelData::getOwnerTable()
		{
			return mOwnerTable;
		}

		void IModelData::setOwnerTable(IModelTable* owner)
		{
			mOwnerTable = owner;
		}

		DataState IModelData::getState()
		{
			return mState;
		}

		void IModelData::setState(DataState state)
		{
			mState = state;
		}

		IPipeDataList& IModelData::getDataList()
		{
			return mDatalist;
		}

		osg::ref_ptr<osg::Node>& IModelData::getModelNode()
		{
			return mModelNode;
		}

		bool IModelData::writeData()
		{
			if (mModelNode.valid())
			{
				String ext = osgDB::getLowerCaseFileExtension(getInnerRecord()->getFeatureUrl());
				if ( ext == "pipe")
				{
					getInnerRecord()->setBoundString(getInnerRecord()->parseBoundString(mModelNode));
					getInnerRecord()->getData() = buildXmlData();
				}
				else if ( ext == "3ds" || ext == "dae")
				{
					return true;
				}
				else
				{
					getInnerRecord()->writeNode(mModelNode,ext);
				}
			}
			return !getInnerRecord()->getData().isNull();
		}

		String ISimpleModelData::generateName()
		{
			FeatureRecord* record = getInnerRecord();
			return record->getTable()  + "_" + StringConverter::toString(record->getGID());
		}

		bool ISimpleModelData::build()
		{
			int num = (int)mDatalist.size();

			if (num == 1)
			{
				mModelNode = mDatalist[0]->buildNode();
			}
			else
			{
				osg::ref_ptr<osg::Group> root = new osg::Group;
				for (int i=0;i<num;i++)
				{
					osg::ref_ptr<osg::Node> g = mDatalist[i]->buildNode();
					if (g.valid())
					{
						root->addChild(g);
					}
				}
				
				mModelNode = root->getNumChildren() > 0 ? root.release() : NULL;
			}

			if (mModelNode.valid())
			{
				double r = mModelNode->getBound().radius();
				if(osg::equivalent(r,0.0) || Math::isNaN(r))
				{
					mModelNode = NULL;
				}
			}

			return mModelNode.valid();
		}

		String IModelData::buildXmlString()
		{
			int num = (int)mDatalist.size();
			TiXmlDocument doc;
			doc.LinkEndChild(new TiXmlDeclaration( "1.0","gb2312",""));
			doc.LinkEndChild(new TiXmlElement("IModelData"));

			doc.RootElement()->SetAttribute("gid",StringConverter::toString(getInnerRecord()->getGID()));
			doc.RootElement()->SetAttribute("dno",StringConverter::toString(getInnerRecord()->getDNO()));
			doc.RootElement()->SetAttribute("table",getInnerRecord()->getTable());

			for (int i=0;i<num;i++)
			{
				doc.RootElement()->LinkEndChild(mDatalist[i]->toXmlElement());
			}

			TiXmlPrinter printer;
			doc.Accept(&printer);
			String xmlString = printer.CStr();
			return xmlString;
		}

		MemoryDataStreamPtr IModelData::buildXmlData()
		{
			StringStream sstream;
			sstream << buildXmlString();
			return MemoryDataStreamPtr(new MemoryDataStream(&sstream));
		}

		void IModelData::appendExt(String ext)
		{
			String url  = getInnerRecord()->getFeatureUrl();

			if (!isGlobalURL() && !StringUtil::endsWith(url,ext))
			{
				getInnerRecord()->setFeatureUrl(url + "." + ext);
			}
		}

		bool IModelData::isGlobalURL()
		{
			return StringUtil::startsWith(getInnerRecord()->getFeatureUrl(),"global://");
		}

		IGroupModelData::IGroupModelData()
		{
			setTileModel(true);
		}

		IGroupModelData::~IGroupModelData()
		{
		}

		String IGroupModelData::generateName()
		{
			return getTable() + "_X" + StringConverter::toString(getRow())  + "_Y" +  StringConverter::toString(getCol());
		}

		void IGroupModelData::onInsert()
		{
			for (IModelDataList::iterator it = mList.begin();
				it!=mList.end();it++)
			{
				(*it)->getInnerRecord()->setTileID(getID());
			}
		}

		long IGroupModelData::getCapacity()
		{
			if (mOwnerTable == NULL || mOwnerTable->getMateRecord()->getEntityMode() == CMetaRecord::EntityWithTileOptimize)
			{
				return 0;
			}

			return (long)mList.size();
		}

		IModelDataList& IGroupModelData::getList()
		{
			return mList;
		}

		MemoryDataStreamPtr IGroupModelData::buildXmlData()
		{
			StringStream sstream;
			for (IModelDataList::iterator it = mList.begin();
				it!=mList.end();it++)
			{
				sstream<<(*it)->buildXmlString();
			}

			return MemoryDataStreamPtr(new MemoryDataStream(&sstream));
		}

		bool IGroupModelData::build()
		{
			if (mOwnerTable == NULL)
			{
				return true;
			}
			if (mOwnerTable->getMateRecord()->getEntityMode() == CMetaRecord::EntityWithTileOptimize)
			{
				return true;
			}
			else
			{
				//bindInfo(new CFeatureInfo);

				CVector3 pOffset = getOffset();
				osg::ref_ptr<osg::Group> g = new osg::Group;
				for (IModelDataList::iterator it = mList.begin();
					it!=mList.end();it++)
				{
					IModelData* d = *it;

					if (pOffset != CVector3::ZERO)
					{
						if (ProjectTool* project = getOwnerTable()->getProjectTool())
						{
							CVector3 center = project->localTo4326(pOffset);
							osg::Matrix matrix = project->computeTransform(pOffset);
							d->doOffset(Vec3ToVector3(matrix.getTrans()));
						}
						else
						{
							d->doOffset(pOffset);
						}
					}

					if (mOwnerTable->getFactory()->build(d))
					{
						osg::ref_ptr<osg::Node> node = d->getModelNode().release();
						if (node.valid())
						{
							node->setName(d->getInnerRecord()->getTable() + "_" + StringConverter::toString(d->getInnerRecord()->getGID()));
							if (d->isGlobalURL())
							{
								osg::ref_ptr<osg::MatrixTransform> matrixtransform = new osg::MatrixTransform;
								CVector3 offset = d->asPipeNode()->getInfo().getModelPosition() - pOffset;
								osg::Matrix matrix = d->getInnerRecord()->getMatrix();
								matrix.setTrans(osg::Vec3d(offset.x,offset.y,offset.z));
								matrixtransform->setMatrix(matrix);
								matrixtransform->addChild(node);
								node = matrixtransform;

								d->getDataList()[0]->setOffset(offset);
								d->getDataList()[0]->setScale(d->getInnerRecord()->getScale());
								d->getDataList()[0]->setRotation(StringConverter::parseQuaternion(d->getInnerRecord()->getRotateString()));
								mDataDistinct |= GLOBAL;
							}
							else
							{

								mDataDistinct |= GEOM;
							}
							d->getInnerRecord()->setBoundString(d->getInnerRecord()->parseBoundString(node.get()));
							d->getInnerRecord()->writeUserValue(node);
							//getFeatureInfo()->getFeatureList().push_back(d->getInnerRecord());
							g->addChild(node);
						}
					}
				}

				if (g->getNumChildren() > 0)
				{
					mModelNode = g;
				}

				return mModelNode.valid();
			}
		}

	}
}
