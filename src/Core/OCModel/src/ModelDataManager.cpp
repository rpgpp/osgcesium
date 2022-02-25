#include "ModelDataManager.h"
#include "PipeTable.h"
#include "OCLayer/OCResource.h"
#include "OCLayer/ReadFileCallback.h"
#include "IModelDatabase.h"
#include "TilesetSaveStrategy.h"
#include "OCMain/LogManager.h"

#define FIND_DRVIER(driver) if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(driver)){sm[rw->className()] = driver;}

namespace OC
{
	namespace Modeling
	{
		ModelDataManager::ModelDataManager(bool createlogfile):
			mSaveStrategyType(SAVE_DB),
			mSavePath(Singleton(Environment).getTempDir()),
			mFinishedTask(0),
			mCommitNum(5000),
			mModelBuildListner(NULL),
			mOCResource(NULL),
			mDoOffset(true),
			mMergeTile(false)
		{
			Singleton(LogManager).createLog("", true, false, true);
			if (osgDB::Registry::instance()->getReadFileCallback() == NULL)
			{
				osgDB::Registry::instance()->setReadFileCallback(new OCReadFileCallback);
			}

			if (SingletonPtr(OCResource) == NULL)
			{
				mOCResource = new OCResource;
			}
		}

		ModelDataManager::~ModelDataManager(void)
		{
			CORE_SAFE_DELETE(mOCResource);
			CORE_SAFE_DELETE(mModelBuildListner);
			destoryMapData();
		}

		void ModelDataManager::reset()
		{
			mCurrentTable = NULL;
			mFinishedTask = 0;
			mAbort = false;
			destoryMapData();
		}

		String ModelDataManager::getLineWhereClause()
		{
			return getStringValue(TableConfig::NameLineClause);
		}

		void ModelDataManager::setLineWhereClause(String clause)
		{
			setConfig(TableConfig::NameLineClause,clause);
		}

		String ModelDataManager::getNodeWhereClause()
		{
			return getStringValue(TableConfig::NameNodeClause);
		}

		void ModelDataManager::setNodeWhereClause(String clause)
		{
			setConfig(TableConfig::NameNodeClause,clause);
		}

		ModelBuildListener* ModelDataManager::getModelBuildListner()
		{
			return mModelBuildListner;
		}

		void ModelDataManager::setModelBuildListner(ModelBuildListener* listner)
		{
			mModelBuildListner = listner;
		}

		SaveStrategy* ModelDataManager::createStrategy(IModelTable* table)
		{
			SaveStrategy* mSaveStrategy = NULL;
			switch(mSaveStrategyType)
			{
			case SAVE_DB:
				{
					SqliteSaveStrategy* strategy = new SqliteSaveStrategy;
					int commitNum = getConmmitNum();
					strategy->setCommitNumber(commitNum);
					mSaveStrategy = strategy;
				}
				break;
			case SAVE_FILE:
				{
					mSaveStrategy = new FileSaveStrategy;
				}
				break;
			case SAVE_XML:
				{
					mSaveStrategy = new XmlSaveStrategy;
				}
				break;
			case SAVE_SHP:
				{
					mSaveStrategy = new ShpFileSaveStrategy;
				}
				break;
			case SAVE_TILESET:
				{
					mSaveStrategy = new TilesetSaveStrategy;
				}
				break;
			case SAVE_SERVERS:
				{
				}
				break;
			default:
				{

				}
				break;
			}

			if (mSaveStrategy)
			{
				mSaveStrategy->setWorkPath(mSavePath);
				mSaveStrategy->setTable(table);
			}

			return mSaveStrategy;
		}

		void ModelDataManager::setRegionClause(CRectangle rect)
		{
			setConfig(TableConfig::NameRegion,rect);
		}

		CRectangle ModelDataManager::getRegionClause()
		{
			return getRectangle(TableConfig::NameRegion);
		}

		void ModelDataManager::setOnlyBuildSpatialIndex(bool spatialonly)
		{
			setConfig(TableConfig::NameBuildOnlySpatialIndex,spatialonly);
		}

		bool ModelDataManager::isOnlyBuildSpatialIndex()
		{
			return getBoolValue(TableConfig::NameBuildOnlySpatialIndex);
		}

		bool ModelDataManager::isAbort()
		{
			return mAbort;
		}

		void ModelDataManager::stop()
		{
			CORE_LOCK_MUTEX(MANAGER_LOCK)
			mFinishedTask = 0;
			mTotalTask = 0;
			mAbort = true;
		}

		bool ModelDataManager::validDatabase()
		{
			return mArchive.valid();
		}

		IModelDatabase* ModelDataManager::getDatabase()
		{
			if (mArchive.valid())
			{
				return static_cast<IModelDatabase*>(mArchive->getDatabase());
			}

			return NULL;
		}

		StringVector ModelDataManager::listModelTable()
		{
			StringVector tables;
			
			if (mArchive.valid())
			{
				getDatabase()->enumMetaTables();

				MetaRecordMapIterator mit = getDatabase()->getMetaTableIterator();
				while(mit.hasMoreElements())
				{
					tables.push_back(mit.peekNextKey());
					mit.moveNext();
				}

				StringVector::iterator it = find(tables.begin(),tables.end(),IModelTable::PipeRackColumnMold);
				if (it != tables.end())
				{
					tables.insert(it,IModelTable::PipeRackColumnReservedMold);
				}

				it = find(tables.begin(),tables.end(),IModelTable::PillarMold);
				if (it != tables.end())
				{
					tables.insert(it,IModelTable::PillarReservedMold);
				}
			}

			return tables;
		}

		StringMap ModelDataManager::enumDatabaseDriver()
		{
			StringMap sm;
			FIND_DRVIER("sqls")
			FIND_DRVIER("psql")
			FIND_DRVIER("horacle")
			FIND_DRVIER("shapefile")
			FIND_DRVIER("db")
			FIND_DRVIER("hcsv")
			return sm;
		}

		ModelDataManager::SourceTypeIterator ModelDataManager::getSurportDatabase()
		{
			if (mSurportSource.empty())
			{
				mSurportSource = enumDatabaseDriver();
			}

			return SourceTypeIterator(mSurportSource.begin(), mSurportSource.end());
		}

		void ModelDataManager::countTask(long i)
		{
			CORE_LOCK_MUTEX(MANAGER_LOCK)
			mFinishedTask+=i;
		}

		long ModelDataManager::getFinishedCount()
		{
			CORE_LOCK_MUTEX(MANAGER_LOCK)
			return mFinishedTask;
		}

		long ModelDataManager::getTotalCount()
		{
			if (mCurrentTable)
			{
				return (long)mCurrentTable->getSize();
			}
			return 0;
		}

		bool ModelDataManager::connect(String connectStr)
		{
			mArchive = dynamic_cast<IArchive*>(osgDB::openArchive(connectStr,osgDB::ReaderWriter::READ));
			if (mArchive.valid())
			{
				mArchive->getDatabase()->setConnectionString(connectStr);
			}
			return  mArchive.valid();
		}

		bool ModelDataManager::connect(String reader,String ip,String database,String user,String password)
		{
			StringStream sstream;
			sstream<<"driver:"<<reader<<";ip:"<<ip<<";catalog:"<<database<<";user:"<<
				user<<";password:"<<password;

			return connect(sstream.str());
		}

		void ModelDataManager::destoryMapData()
		{
			CORE_DELETE_SECOND(mPipeTableMap,ModelTableMap);
		}

		void ModelDataManager::setSaveStrategy(SaveStrategyType type)
		{
			mSaveStrategyType = type;
		}

		SaveStrategyType ModelDataManager::getSaveStrategy()
		{
			return mSaveStrategyType;
		}

		void ModelDataManager::setExtension(String extension)
		{
			setConfig(TableConfig::NameExtension,extension);
		}

		String ModelDataManager::getExtension()
		{
			return getStringValue(TableConfig::NameExtension,"pipe");
		}

		void ModelDataManager::setSavePath(String path)
		{
			mSavePath = path;
		}

		String ModelDataManager::getSavePath()
		{
			return mSavePath;
		}

		CCoreConfig& ModelDataManager::getKeyConfig()
		{
			return mManagerConfig;
		}

		void ModelDataManager::setDoOffset(bool dooffset)
		{
			mDoOffset = dooffset;
		}

		bool ModelDataManager::getDoOffset()
		{
			return mDoOffset;
		}

		void ModelDataManager::setMergeTile(bool mergetile)
		{
			mMergeTile = mergetile;
			IPipeDataBuilder::setLowQuality();
		}

		bool ModelDataManager::getMergeTile()
		{
			return mMergeTile;
		}

		void ModelDataManager::setCommitNum(int num)
		{
			mCommitNum = num;
		}

		int ModelDataManager::getConmmitNum()
		{
			return mCommitNum;
		}

		void ModelDataManager::setOptimizeLevel(int level)
		{
			setConfig(TableConfig::NameOptimizeLevel,level);
		}

		int ModelDataManager::getOptimizeLevel()
		{
			return getIntValue(TableConfig::NameOptimizeLevel);
		}

		void ModelDataManager::setLayerVisibleDis(double dis)
		{
			setConfig(TableConfig::NameRangeMax,dis);
		}

		double ModelDataManager::getLayerVisibleDis()
		{
			return getDoubleValue(TableConfig::NameRangeMax,1000.0);
		}

		void ModelDataManager::setConstElevation(double elev)
		{
			setConfig(TableConfig::NameConstantElevation,elev);
		}

		double ModelDataManager::getConstElevation()
		{
			return getDoubleValue(TableConfig::NameConstantElevation,-999.0);
		}

		void ModelDataManager::setUnderNodePlusHeight(double height)
		{
			setConfig(TableConfig::NamePlusUnderGroundNode,height);
		}

		double ModelDataManager::getUnderNodePlusHeight()
		{
			return getDoubleValue(TableConfig::NamePlusUnderGroundNode);
		}

		void ModelDataManager::build()
		{
			ModelTableMap::iterator it = mPipeTableMap.begin();
			for (;it!=mPipeTableMap.end();it++)
			{
				if (isAbort())
				{
					return;
				}

				mCurrentTable = it->second;

				mXmlConfig.decorate(mCurrentTable);

				mCurrentTable->getConfig()->setConfig(TableConfig::NamePlusUnderGroundNode,getUnderNodePlusHeight());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameConstantElevation,getConstElevation());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameExtension,getExtension());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameBuildOnlySpatialIndex,isOnlyBuildSpatialIndex());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameConnectString,getDatabase()->getConnectionString());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameOptimizeLevel,getOptimizeLevel());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameNodeClause,getNodeWhereClause());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameLineClause,getLineWhereClause());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameRangeMax,getLayerVisibleDis());
				mCurrentTable->getConfig()->setConfig(TableConfig::NameRegion, getRegionClause());
				
				bool dofix = mManagerConfig.getBoolValue(TableConfig::NameFixNullSectLine, true);
				mCurrentTable->getConfig()->setConfig(TableConfig::NameFixNullSectLine, dofix);

				if (mManagerConfig.keyExists("prjcs"))
				{
					String prjcs = mManagerConfig.getStringValue("prjcs");
					//mCurrentTable->setProjectTool(new ProjectTool("+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +ellps=krass +towgs84=15.8,-154.4,-82.3,0,0,0,0 +units=m +no_defs"));
					mCurrentTable->setProjectTool(new ProjectTool(prjcs));
				}
				
				mCurrentTable->startBuild();

				if (mModelBuildListner)
				{
					mModelBuildListner->onFinished(mCurrentTable);
				}

				mFinishedTask = 0;
			}
		}

		void ModelDataManager::setList(StringVector tableList)
		{
			reset();
			StringVector::iterator it = tableList.begin();
			for(;it!=tableList.end();it++)
			{
				String tableName = *it;

				IModelTable* pModelTable;
				
				{
					pModelTable = new PipeTable(this);
				}

				pModelTable->setName(tableName);
				mPipeTableMap[tableName] = pModelTable;
			}
		}

		bool ModelDataManager::loadConfig(String path)
		{
			return mXmlConfig.loadXml(path);
		}

		CXmlConfig& ModelDataManager::getXmlConfig()
		{
			return mXmlConfig;
		}

	}
}


