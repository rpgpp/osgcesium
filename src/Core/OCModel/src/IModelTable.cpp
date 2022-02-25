#include "IModelTable.h"
#include "ModelDataManager.h"
#include "IModelFactory.h"
#include "PipeModelFactory.h"

namespace OC
{
	namespace Modeling
	{
		String IModelTable::TableOverGroundPipe			= "地上管网"; 
		String IModelTable::TableUnderGroundPipe			= "地下管网"; 
		String IModelTable::ShelfOverGround				= "地上管架";
		String IModelTable::ShelfUnderGround				= "地下管廊";

		//地上管廊模型组分类型
		String IModelTable::PillarMold						= "桁架小立柱";
		String IModelTable::PillarReservedMold			= "桁架预留小立柱";
		String IModelTable::PileFoundationMold			= "桩点";
		String IModelTable::CushionMold					= "垫层";
		String IModelTable::PileCapMold					= "承台";
		String IModelTable::GroundBeamMold					= "地梁";
		String IModelTable::PipeRackColumnMold			= "管架立柱";
		String IModelTable::PipeRackColumnReservedMold	= "管架预留立柱";
		String IModelTable::PipeRackBeams					= "管架梁";
		String IModelTable::PipeRackBeamsReserved			= "管架预留梁";		
		String IModelTable::TrussBeamMold					= "桁架横梁";
		String IModelTable::TrussBeamReservedMold			= "桁架预留横梁";
		String IModelTable::TrussStringerMold				= "桁架纵梁";
		String IModelTable::TrussStringerReservedMold	= "桁架预留纵梁";
		String IModelTable::TensionBrace					= "拉撑";
		String IModelTable::TensionBraceReserved			= "预留拉撑";
		String IModelTable::PipeShoeMold					= "管托";
		String IModelTable::BracketMold					= "牛腿";
		String IModelTable::LadderMold						= "爬梯";
		String IModelTable::WalkerLandingMold				= "走道平台";

		IModelTable::IModelTable(ModelDataManager* owner)
			:mType(TableUnderGroundPipe),
			mDataManager(owner),
			mSaveStrategy(NULL),
			mFactory(NULL)
		{
			mMetaRecord = new CMetaRecord;
		}

		IModelTable::~IModelTable(void)
		{
			CORE_SAFE_DELETE(mFactory);
		}

		void IModelTable::outputBlock(IModelDataList list)
		{
			IModelDataList::iterator it = list.begin();
			for (;it!=list.end();it++)
			{
				IModelData* data = *it;
#if _DEBUG
				long gid = data->getInnerRecord()->getGID();
#endif

				if (mDataManager->isAbort())
				{
					return;
				}

				data->appendExt(getConfig()->getStringValue(TableConfig::NameExtension));

				if (mDataManager->getDoOffset())
				{
					CVector3 center = data->getCenter();
					data->doOffset(center);
				}

				bool ok_to_write = getConfig()->getBoolValue(TableConfig::NameBuildOnlySpatialIndex);

				if (ok_to_write == false)
				{
					ok_to_write = mFactory->build(data);
				}

				if (ok_to_write)
				{
					data->writeData();
					mSaveStrategy->write(data);

					if (getMateRecord()->getEntityMode() == CMetaRecord::EntityWithTileOptimize)
					{
						if (IGroupModelData* gmd = data->isGroupModelData())
						{
							outputBlock(gmd->getList());
						}
					}
				}

				mDataManager->countTask(data->getCapacity());

				data->clear();
			}
		}

		CMetaRecord* IModelTable::getMateRecord()
		{
			return mMetaRecord.get();
		}

		bool IModelTable::init()
		{
			CORE_SAFE_DELETE(mFactory);
			mFactory = findSuitableFactory(this);

			if (mFactory == NULL)
			{
				return false;
			}

			mSaveStrategy = mDataManager->createStrategy(this);

			if (mSaveStrategy == NULL)
			{
				return false;
			}

			int level = Math::Clamp(getConfig()->getIntValue(TableConfig::NameOptimizeLevel),0,12);
			mMetaRecord->setLevel(level);
			if(level > 0)
			{
				mMetaRecord->setEntityMode(mDataManager->getMergeTile() ? CMetaRecord::EntityWithMutiRecord : CMetaRecord::EntityWithTileOptimize);

			}
			
			mMetaRecord->setInstanceArchive(false);
			if (getConfig()->getBoolValue(TableConfig::NameBuildOnlySpatialIndex))
			{
				mMetaRecord->setDataFrom(DATA_FROM_RAW_DATABASE);
				String connectStr = getConfig()->getStringValue(TableConfig::NameConnectString);
				mMetaRecord->setConnectString(connectStr);
			}

			return mSaveStrategy->init();
		}

		SaveStrategy* IModelTable::getSaveStratety()
		{
			return mSaveStrategy.get();
		}

		void IModelTable::setProjectTool(ProjectTool* project)
		{
			mProjectTool = project;
		}

		ProjectTool* IModelTable::getProjectTool()
		{
			return mProjectTool.get();
		}

		osg::ref_ptr<CDataFilter>& IModelTable::getFilter()
		{
			return mDataFilter;
		}

		void IModelTable::setName(String name)
		{
			mName = name;
		}

		String IModelTable::getName()
		{
			return mName;
		}

		void IModelTable::setType(String type)
		{
			mType = type;
		}

		String IModelTable::getType()
		{
			return mType;
		}

		void IModelTable::setConfig(TableConfig config)
		{
			mConfig = config;
		}

		TableConfig* IModelTable::getConfig()
		{
			return &mConfig;
		}

		void IModelTable::setFactory(IModelFactory* factory)
		{
			mFactory = factory;
		}

		IModelFactory* IModelTable::findSuitableFactory(IModelTable* table)
		{
			IModelFactory* factory = NULL;
			String tableType = table->getType();
			if (IModelTable::TableUnderGroundPipe == tableType || IModelTable::TableOverGroundPipe == tableType)
			{
				factory = new PipeModelFactory;
			}

			return factory;
		}
	}
}

