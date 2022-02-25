#ifndef _IMODELTABLE_CLASS_H_
#define _IMODELTABLE_CLASS_H_

#include "TableConfig.h"
#include "OCLayer/MetaRecord.h"
#include "SaveStrategy.h"
#include "DataFilter.h"
#include "ProjectTool.h"

#pragma warning(disable:4355)

namespace OC
{
	namespace Modeling
	{
		class _ModelExport IModelTable : public osg::Referenced
		{
		public:
			IModelTable(ModelDataManager* owner);
			virtual ~IModelTable(void);

			virtual GenericMoldTable* asGenericMoldTable(){return NULL;}
			virtual PipeTable* asPipeTable(){return NULL;}

			virtual bool startBuild(){return false;}
			virtual long getSize(){return 0;}

			void setConfig(TableConfig config);
			TableConfig* getConfig();

			void   setName(String name);
			String getName();

			void   setType(String type);
			String getType();

			virtual bool checkData(){return true;}
			virtual void push_data(IModelData* data){}
			virtual CMetaRecord* getMateRecord();

			virtual IModelFactory* getFactory(){return mFactory;}
			void setFactory(IModelFactory* factory);

			static IModelFactory* findSuitableFactory(IModelTable* table);

			SaveStrategy* getSaveStratety();

			void setProjectTool(ProjectTool* project);
			ProjectTool* getProjectTool();

			osg::ref_ptr<CDataFilter>& getFilter();

		protected:
			virtual bool init();
			void outputBlock(IModelDataList list);
			virtual IModelDataList getBlockMap() {return IModelDataList();}
		protected:
			String								mType;
			String								mName;
			TableConfig							mConfig;
			IModelFactory*						mFactory;
			ModelDataManager*					mDataManager;
			osg::ref_ptr<CMetaRecord>			mMetaRecord;
			osg::ref_ptr<CDataFilter>			mDataFilter;
			osg::ref_ptr<SaveStrategy>			mSaveStrategy;
			osg::ref_ptr<ProjectTool>			mProjectTool;
		public:
			static String TableOverGroundPipe;
			static String TableUnderGroundPipe;
			static String ShelfOverGround;
			static String ShelfUnderGround;

			//地上管廊模型组分类型
			static String PillarMold;
			static String PillarReservedMold;
			static String PileFoundationMold;
			static String CushionMold;
			static String PileCapMold;
			static String GroundBeamMold;
			static String PipeRackColumnMold;
			static String PipeRackColumnReservedMold;
			static String PipeRackBeams;
			static String PipeRackBeamsReserved;		
			static String TrussBeamMold;
			static String TrussBeamReservedMold;
			static String TrussStringerMold;
			static String TrussStringerReservedMold;
			static String TensionBrace;
			static String TensionBraceReserved;
			static String PipeShoeMold;
			static String BracketMold;
			static String LadderMold;
			static String WalkerLandingMold;
		};

		typedef std::map<String,IModelTable*>	ModelTableMap;
	}
}



#endif


