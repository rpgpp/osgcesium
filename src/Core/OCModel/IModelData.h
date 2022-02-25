#ifndef _IPIPEDATA_CLASS_H_
#define _IPIPEDATA_CLASS_H_

#include "ModelDefine.h"
#include "PipeData.h"

namespace OC
{
	namespace Modeling
	{
		enum DataState
		{
			STATE_NORMAL,
			STATE_ONLY_FOR_CALC,
			STATE_ERROR
		};

		class _ModelExport IModelData
		{
		public:
			IModelData();
			virtual ~IModelData();
			virtual FeatureRecord* getInnerRecord(){return NULL;}
			virtual MemoryDataStreamPtr buildXmlData();

			virtual CVector3 getWorldCenter() { return mCenter; }
			virtual CVector3 getCenter() { return mCenter; }
			virtual void setCenter(CVector3 center) { mCenter = center; }

			virtual PipeLine*					asPipeLine(){return NULL;}
			virtual PipeNode*				asPipeNode(){return NULL;}
			virtual GenericMoldData*	asMoldData(){return NULL;}
			virtual IGroupModelData*	isGroupModelData(){return NULL;}
			virtual ISimpleModelData*	isSimpleModelData(){return NULL;}


			virtual long getCapacity(){return 1;}

			virtual void doOffset(CVector3 offset){getInnerRecord()->setOffset(offset);}

			CVector3 getWorldZVector();
			void setWorldZVector(CVector3 direction);

			bool writeData();

			IModelTable* getOwnerTable();
			void setOwnerTable(IModelTable* owner);

			DataState	getState();
			void setState(DataState state);

			IPipeDataList& getDataList();

			virtual bool build(){return false;}
			virtual void appendExt(String ext);
			osg::ref_ptr<osg::Node>& getModelNode();

			String buildXmlString();


			void clear()
			{
				getModelNode() = NULL;
				getInnerRecord()->getData().setNull();
			}

			bool isGlobalURL();

			void setTemplateRecord(TemplateRecord* templaterecord);
			TemplateRecord* getTemplateRecord();
		protected:
			CVector3						mCenter;
			CVector3						mWorldZVector;
			DataState						mState;
			IModelTable*					mOwnerTable;
			IPipeDataList					mDatalist;
			osg::ref_ptr<TemplateRecord>	mTemplateRecord;
			osg::ref_ptr<osg::Node>			mModelNode;
		};

		class _ModelExport ISimpleModelData : public IModelData , public FeatureRecord
		{
		public:
			ISimpleModelData(){}
			virtual ~ISimpleModelData(){}
			virtual String generateName();
			virtual FeatureRecord* getInnerRecord(){return this;}
			virtual bool build();
			virtual ISimpleModelData*	isSimpleModelData(){return static_cast<ISimpleModelData*>(this);}
			virtual void doOffset(CVector3 offset){IModelData::doOffset(offset);}
		};

		class _ModelExport IGroupModelData : public IModelData , public FeatureRecord
		{
		public:
			IGroupModelData();
			virtual ~IGroupModelData();
			virtual FeatureRecord* getInnerRecord(){return this;}
			virtual String generateName();
			virtual bool build();
			virtual IGroupModelData*	isGroupModelData(){return static_cast<IGroupModelData*>(this);}

			virtual MemoryDataStreamPtr buildXmlData();
			virtual long getCapacity();
			virtual void onInsert();

			int getRow() { return row; }
			int getCol() { return col; }
			int getLevel() { return level; }

			void setRow(int v) { row = v; }
			void setCol(int v) { col = v; }
			void setLevel(int v) { level = v; }

			int row = 0;
			int col = 0;
			int level = 0;

			IModelDataList& getList();

			enum DataDistinct
			{
				NONE = (1 << 0),
				GEOM = (1 << 1),
				GLOBAL = (1 << 2),
			};
			uint32	mDataDistinct = NONE;
		private:
			IModelDataList  mList;
		};
	}
}

#endif
