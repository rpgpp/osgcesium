#ifndef _IDATABASE_CALSS_H__
#define _IDATABASE_CALSS_H__

#include "IDatabaseObject.h"
#include "QueryInfo.h"

namespace OC
{
	class _LayerExport IDatabase : public osg::Object
	{
	public:
		enum OperType
		{
			OP_ENTITY,
			OP_MODEL
		};
		IDatabase(void){}
		IDatabase(const IDatabase& eh,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY){}
		virtual ~IDatabase(void){}
		META_Object(OCDatabase,IDatabase);

		virtual bool connect(String connectStr){return false;}

		virtual bool createTable(String name,OCHeaderInfo* headinfo){return false;}
		virtual bool deleteTable(String name){return false;}
		virtual bool existTable(String name){return false;}
		virtual bool createDatabase(String name){return false;}
		virtual bool deleteDatabase(String name){return false;}
		virtual bool existDatabase(String name){return false;}

		virtual bool insert(String table,TemplateRecord* record){return false;}
		virtual bool update(String table,TemplateRecord* record,String whereClause = StringUtil::BLANK){return false;}
		virtual TemplateRecordList query(String table,OCHeaderInfo* headinfo,CQueryInfo* filter = NULL){return TemplateRecordList();}
		virtual OCHeaderInfo* getHeadInfo(String table){return NULL;}
		
		virtual StringVector simplequery(String sqlStr){return StringVector();}
		virtual StringVector enumDatabase(){return StringVector();}

		virtual bool getModelStream(String featureURL,StringStream& stream){return false;}
		virtual bool getTextureStream(String texture,StringStream& stream){return false;}

		virtual bool create(String url,String user = "",String psw = ""){return false;}
		virtual bool open(String url,String user = "",String psw = ""){return false;}
		virtual long insert(FeatureRecord* record, OperType ot){return -1;}
		virtual bool update(FeatureRecord* record){return false;}
		virtual bool query(FeatureRecord* record){return false;}
		virtual bool query(std::vector<osg::ref_ptr<FeatureRecord> >& list,osgDB::Options* options = NULL){return false;}

		//meta
		virtual long insert(CMetaRecord* record){return -1;}
		virtual bool query(CMetaRecord* record){return false;}
		virtual bool update(CMetaRecord* record){return false;}

		//tile

		virtual bool excute(String sql,String& result){return false;}

		virtual MemoryDataStreamPtr readTextureData(String textrueName){return MemoryDataStreamPtr();}
		
		virtual bool insertTexture(String name,MemoryDataStreamPtr dataStream){return false;}

		virtual bool beginTransaction(){return false;}
		virtual bool commitTransaction(){return false;}
		virtual bool rollbackTransaction(){return false;}

		void setConnectionString(String connect){mConnectionString = connect;}
		String getConnectionString(){return mConnectionString;}

		virtual CRectangle calculate_layer_extent(String layername){return CRectangle();}
	protected:
		String mConnectionString;
	};
}

#endif
