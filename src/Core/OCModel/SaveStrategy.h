#ifndef _SAVE_STRATEGY_CLASS_H_
#define _SAVE_STRATEGY_CLASS_H_

#include "IModelData.h"

namespace OC
{
	namespace Modeling
	{
		enum SaveStrategyType {
			SAVE_DB, SAVE_SERVERS, SAVE_FILE, SAVE_XML, SAVE_SHP, SAVE_TILESET
		};
		class _ModelExport SaveStrategy : public osg::Referenced
		{
		public:
			SaveStrategy(void);
			virtual ~SaveStrategy(void);
			virtual bool init(){return false;}
			virtual void setWorkPath(String path);
			virtual String getWorkPath(){return mWorkPath;}
			virtual bool write(IModelData* data){return writeFeature(data->getInnerRecord());}
			virtual bool writeFeature(FeatureRecord* record){return false;}
			virtual void insertMetadata(CMetaRecord* record);

			void setTable(IModelTable* table);
			void setCommitNumber(int num);
		protected:
			int							mCommitNumber;
			String						mWorkPath;
			IModelTable*				mRefTable;
			osg::ref_ptr<IArchive>		mArchive;
		};

		class _ModelExport SqliteSaveStrategy : public SaveStrategy
		{
		public:
			SqliteSaveStrategy();
			~SqliteSaveStrategy();
			virtual bool init();
			virtual bool writeFeature(FeatureRecord* record);
		private:
		};

		class _ModelExport XmlSaveStrategy:public SaveStrategy
		{
		public:
			XmlSaveStrategy();
			~XmlSaveStrategy();
			virtual bool init();
			virtual bool writeFeature(FeatureRecord* record);
		private:
			void writeRecord(FeatureRecord* record);
			String			mXmlFile;
			TiXmlElement  * mLayerElement;
			TiXmlDocument * mDocument;			
		};

		class _ModelExport ShpFileSaveStrategy:public SaveStrategy
		{
		public:
			ShpFileSaveStrategy	();
			~ShpFileSaveStrategy();

			virtual bool	init();
			virtual bool	writeFeature(FeatureRecord* record);
		};

		class _ModelExport FileSaveStrategy : public SaveStrategy
		{
		public:
			FileSaveStrategy();
			~FileSaveStrategy();
			virtual bool init();
			virtual bool writeFeature(FeatureRecord* record);
		};

		class _ModelExport SqlServerSaveStrategy : public SaveStrategy
		{
		public:
			SqlServerSaveStrategy(osgDB::Archive* archive){}
			~SqlServerSaveStrategy(){}
		protected:
			osg::observer_ptr<osgDB::Archive> mArchive;
		};

		class _ModelExport OracleSaveStrategy : public SaveStrategy
		{
		public:
			OracleSaveStrategy(){}
			~OracleSaveStrategy(){}
		};
	}
}


#endif


