#include "PipeTable.h"
#include "ModelDataManager.h"
#include "PipeModelFactory.h"
#include "IModelDatabase.h"

namespace OC{
	namespace Modeling{
	
	PipeTable::PipeTable(ModelDataManager* owner)
		:mNodeTable(this)
		,mLineTable(this)
		,IModelTable(owner)
	{
	}

	PipeTable::~PipeTable(void)
	{
		
	}
	
	PipeTable* PipeTable::from(IArchive* archive)
	{
		return NULL;
	}


	void PipeTable::setQueryMapping(FieldMapping* mapping)
	{
		mFieldMapping = mapping;
	}

	FieldMapping* PipeTable::getQueryMapping()
	{
		return mFieldMapping.get();
	}

	long PipeTable::getSize()
	{
		return (long)mNodeTable.getNodeMap().size() + (long)mLineTable.getLineMap().size();
	}

	bool PipeTable::getData()
	{
		String lineclause = getConfig()->getStringValue(TableConfig::NameLineClause);
		if (!lineclause.empty())
		{
			mLineTable.getFilter() = new CDataFilter;
			mLineTable.getFilter()->setLineWhereClause(lineclause);
		}

		String nodeclause = getConfig()->getStringValue(TableConfig::NameNodeClause);
		if (!nodeclause.empty())
		{
			mNodeTable.getFilter() = new CDataFilter;
			mNodeTable.getFilter()->setNodeWhereClause(nodeclause);
		}

		CRectangle extent = getConfig()->getRectangle(TableConfig::NameRegion);
		if (extent != CRectangle::ZERO)
		{
			mLineTable.getFilter() = new CDataFilter;
			mLineTable.getFilter()->setExtent(extent);
		}

		mFieldMapping = getConfig()->getFieldMapping();

		if (mFieldMapping == NULL)
		{
			LOG << getName() + "未配置管网规则!";
			return false;
		}

		mMetaRecord->setDBType(CMetaRecord::PipeDatabase);
		mMetaRecord->setLineHeader(mFieldMapping->getLineQueryHeader());
		mMetaRecord->setNodeHeader(mFieldMapping->getNodeQueryHeader());

		getConfig()->setConfig(TableConfig::NameBuildIsolatePoint,!mNodeTable.getFilter().valid());

		bool result = mDataManager->getDatabase()->getData(this);

		if (result)
		{
			result = checkData();
		}

		return result;
	}

	bool PipeTable::init()
	{
		String netName = getName();
		mMetaRecord->setNetName(netName);
		mNodeTable.setNameTable(netName + NodeTableSubfix);
		mLineTable.setNameTable(netName + LineTableSubfix);

		String netAliasName = getConfig()->getDisplayName();
		mMetaRecord->setAliasName(netAliasName);
		mNodeTable.setAliasName(netAliasName + "_点");
		mLineTable.setAliasName(netAliasName + "_线");

		mMetaRecord->setMeasureZ(getConfig()->getStringValue(TableConfig::NameZFlag));

		return IModelTable::init();
	}

	void PipeTable::trans2world()
	{
		if (ProjectTool* project = getProjectTool())
		{
			CRectangle extent;

			{
				NodeMapIterator it = mNodeTable.getIterator();
				while (it.hasMoreElements())
				{
					PipeNode* node = it.getNext();
					CVector3 LonLatH = project->localTo4326(node->getCenter());
					extent.merge(CVector2(LonLatH.x, LonLatH.y));
					node->setCenter(LonLatH);
					project->transNode2Global(node);
				}
			}

			{
				LineMapIterator it = mLineTable.getIterator();
				while (it.hasMoreElements())
				{
					PipeLine* line = it.getNext();
					CVector3 LonLatH = project->localTo4326(line->getCenter());
					extent.merge(CVector2(LonLatH.x, LonLatH.y));
					line->setCenter(LonLatH);
					project->transLine2Global(line);
				}
			}

			mMetaRecord->setExtent(extent);
		}
	}

	void PipeTable::output()
	{
		outputBlock(mLineTable.getBlockMap());
		outputBlock(mNodeTable.getBlockMap());
	}

	bool PipeTable::startBuild()
	{
		reset();

		bool result = init();

		if (!result)
		{
			reset();
			LOG << mDataManager->getSavePath() + "/" + getName() + "文件正在被占用!";
			return result;
		}

		result = prepare();

		if (!result)
		{
			LOG << getName() + "管网数据获取失败!";
			return result;
		}

		trans2world();
		output();

		reset();

		return result = true;
	}

	void PipeTable::reset()
	{
		mLineTable.clear();
		mNodeTable.clear();
		mSaveStrategy = NULL;
	}

	bool PipeTable::checkData()
	{
		mLineTable.removeInvalidData();
		mNodeTable.removeInvalidData();

		bool result = mNodeTable.checkData();

		if (!result)
		{
			return false;
		}

		result = mLineTable.checkData();

		if (!result)
		{
			return false;
		}

		return true;
	}

	NodeTable& PipeTable::getNodeTable()
	{
		return mNodeTable;
	}

	LineTable& PipeTable::getLineTable()
	{
		return mLineTable;
	}

	void PipeTable::fixData()
	{
		mLineTable.fixData();
		mNodeTable.fixData();
	}

	void PipeTable::push_data(IModelData* data)
	{
		if (PipeLine* line = data->asPipeLine())
		{
			getLineTable().push(line);
		}
		else if (PipeNode* node = data->asPipeNode())
		{
			getNodeTable().push(node);
		}
	}

	void PipeTable::onPush(IModelData* data)
	{
		if (mModelPushListener.valid())
		{
			mModelPushListener->onPush(data);
		}
	}

	void PipeTable::setPushListener(ModelPushListener* listener)
	{
		mModelPushListener = listener;
	}

	bool PipeTable::prepare()
	{
		bool result = getData();

		CRectangle pNetBound = mNodeTable.getExtent().unionRect(mLineTable.getExtent());

		pNetBound.getMaximum().x += 1.0;
		pNetBound.getMaximum().y += 1.0;
		pNetBound.getMinimum().x -= 1.0;
		pNetBound.getMinimum().y -= 1.0;

		mMetaRecord->setExtent(pNetBound);

		return result;
	}

}
}
