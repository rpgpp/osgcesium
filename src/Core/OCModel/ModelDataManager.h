#ifndef _PIPE_DATA_MANAGER_CLASS_H_
#define _PIPE_DATA_MANAGER_CLASS_H_

#include "xmlConfig.h"
#include "IModelTable.h"
#include "SaveStrategy.h"

namespace OC
{
	class OCResource;
	namespace Modeling
	{
		class _ModelExport ModelBuildListener
		{
		public:
			virtual void onPrepare(IModelTable* table){}
			virtual void onFinished(IModelTable* table){}
		};

		class _ModelExport ModelDataManager : protected TableConfig
		{
		public:
			ModelDataManager(bool createlogfile = true);
			~ModelDataManager(void);

			bool validDatabase();
			IModelDatabase* getDatabase();

			bool connect(String connectStr);
			bool connect(String reader,String ip,String database,String user,String password);
			bool loadConfig(String path);

			void setSaveStrategy(SaveStrategyType type);
			SaveStrategyType getSaveStrategy();

			void setExtension(String ext);
			String getExtension();

			void setSavePath(String path);
			String getSavePath();

			void setCommitNum(int num);
			int getConmmitNum();

			void setLayerVisibleDis(double dis);
			double getLayerVisibleDis();

			void setConstElevation(double elev);
			double getConstElevation();

			void setUnderNodePlusHeight(double height);
			double getUnderNodePlusHeight();

			void setOptimizeLevel(int level);
			int getOptimizeLevel();

			String getLineWhereClause();
			void setLineWhereClause(String clause);

			String getNodeWhereClause();
			void setNodeWhereClause(String clause);

			void setOnlyBuildSpatialIndex(bool spatialonly);
			bool isOnlyBuildSpatialIndex();

			void setRegionClause(CRectangle rect);
			CRectangle getRegionClause();

			ModelBuildListener* getModelBuildListner();
			void setModelBuildListner(ModelBuildListener* listner);

			void countTask(long i = 1);
			long getFinishedCount();
			long getTotalCount();

			void stop();
			void reset();
			void build();
			void setList(StringVector tableList);

			bool isAbort();

			static StringMap enumDatabaseDriver();
			typedef MapIterator<StringMap> SourceTypeIterator;
			SourceTypeIterator getSurportDatabase();

			SaveStrategy* createStrategy(IModelTable* table);

			CXmlConfig&		 getXmlConfig();
			StringVector	 listModelTable();

			void setDoOffset(bool dooffset);
			bool getDoOffset();

			void setMergeTile(bool mergetile);
			bool getMergeTile();

			CCoreConfig& getKeyConfig();
		protected:
			void destoryMapData();
		protected:
			int									mCommitNum;
			bool								mDoOffset;
			bool								mMergeTile;
			long								mFinishedTask;
			long								mTotalTask;
			bool								mAbort;
			String								mSavePath;
			CCoreConfig							mManagerConfig;
			StringMap							mSurportSource;
			SaveStrategyType					mSaveStrategyType;
			osg::ref_ptr<IArchive>				mArchive;
		private:
			CXmlConfig							mXmlConfig;
			IModelTable*						mCurrentTable;
			ModelTableMap						mPipeTableMap;		
			OCResource*						mOCResource;
			ModelBuildListener*					mModelBuildListner;
			CORE_MUTEX(MANAGER_LOCK)
		};
	}
}

#endif





