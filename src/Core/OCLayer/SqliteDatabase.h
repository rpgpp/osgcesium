#ifndef _SQLITE_DATABASE_H_
#define _SQLITE_DATABASE_H_

#include "OCMain/IDatabase.h"
#include "FeatureRecord.h"

namespace OC
{
	class _LayerExport SqliteDatabase : public IDatabase
	{
	public:
		SqliteDatabase(void);
		virtual ~SqliteDatabase(void);

		virtual bool getModelStream(String featureURL,StringStream& stream);
		virtual bool getTextureStream(String texture,StringStream& stream);

		//
		virtual bool create(String url,String user = "",String psw = "");
		virtual bool open(String url,String user = "",String psw = "");
		virtual bool update(String table, TemplateRecord* record, String whereClause = StringUtil::BLANK);
		virtual TemplateRecordList query(String table, OCHeaderInfo* headinfo, CQueryInfo* filter = NULL);
		virtual OCHeaderInfo* getHeadInfo(String table);

		//feature
		virtual long insert(FeatureRecord* record,OperType ot);

		virtual bool query(FeatureRecord* record);
		virtual bool query(FeatureRecordList& list,osgDB::Options* options = NULL);
		virtual bool update(FeatureRecord* record);

		//meta
		virtual long insert(CMetaRecord* record);
		virtual bool query(CMetaRecord* record);
		virtual bool update(CMetaRecord* record);

		virtual bool insertTexture(String name,MemoryDataStreamPtr dataStream);
		virtual MemoryDataStreamPtr readTextureData(String textrueName);

	public:
		virtual bool connect(String connectStr);
		virtual bool beginTransaction();
		virtual bool commitTransaction();
		virtual bool rollbackTransaction();
		virtual bool excute(String sql,String& result);
		virtual StringVector simplequery(String sqlStr);
		virtual bool insert(String table, TemplateRecord* record);

		virtual bool createTable(String name, OCHeaderInfo* headinfo);
		virtual bool existTable(String table);
		bool isSpatialite() { return mIsSpatialite; }
		virtual StringVector enumGeometryLayers();
		virtual void enumSpatialLayers();
		virtual String getDatabaseTime();
		virtual TemplateRecordList selectTable(String table);
		void checkSpatialLayers();
		virtual StringVector enumNets();
	private:
		//osg::ref_ptr<IDatabase>				mSpatialiteDatabase;
		bool								mIsTransaction;
		bool								mIsSpatialite;
		void init_meta();
		SQLite*								mSqlite;
		osg::ref_ptr<OCHeaderInfo>		mEntityHeader;
		osg::ref_ptr<OCHeaderInfo>		mModelHeader;
		osg::ref_ptr<OCHeaderInfo>		mLayerHeader;
		RefObjectCacher<OCSpatialTable> mSpatialTables;
	};

}

#endif
