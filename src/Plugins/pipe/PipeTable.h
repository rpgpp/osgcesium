#ifndef _OC_Model_PipeTable_H__
#define _OC_Model_PipeTable_H__

#include "OCModel/IModelTable.h"
#include "NodeTable.h"
#include "LineTable.h"
#include "OCModel/SaveStrategy.h"

namespace OC
{
	namespace Modeling
	{
		class PipeTable : public IModelTable
		{
		public:
			PipeTable(ModelDataManager* owner);
			~PipeTable(void);
			virtual PipeTable* asPipeTable(){return static_cast<PipeTable*>(this);}

			static PipeTable* from(IArchive* archive);

			virtual bool startBuild();
			virtual long getSize();

			NodeTable& getNodeTable();
			LineTable& getLineTable();

			void fixData();

			bool prepare();

			void setQueryMapping(FieldMapping* mapping);
			FieldMapping* getQueryMapping();

			bool checkData();

			virtual void push_data(IModelData* data);
			void onPush(IModelData* data);
			void setPushListener(ModelPushListener* listener);
		protected:
			void reset();
			bool init();
			bool getData();

			void trans2world();
			void output();
		protected:
			osg::ref_ptr<ModelPushListener> mModelPushListener;
			osg::ref_ptr<FieldMapping>	mFieldMapping;
			LineTable					mLineTable;
			NodeTable					mNodeTable;
			CORE_FRIEND_CLASS(LineTable);
			CORE_FRIEND_CLASS(NodeTable);
		};

		typedef	std::map<String,PipeTable*> PipeTableMap;
	}
}


#endif


