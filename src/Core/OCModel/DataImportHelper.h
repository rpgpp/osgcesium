#ifndef _PIPEDATA_IMPORT_HELPER_H_
#define _PIPEDATA_IMPORT_HELPER_H_

#include "IModelDatabase.h"

namespace OC
{
	namespace Modeling
	{
		class CDataImportHelper;
		class _ModelExport CDataImportListener : public osg::Referenced
		{
		public:
			CDataImportListener(){}
			~CDataImportListener(){}
			virtual void onBegin(CDataImportHelper* helper){}
			virtual void onInsert(CDataImportHelper* helper){}
			virtual void onFinish(CDataImportHelper* helper){}
		};

		class _ModelExport IRecordTransform : public osg::Referenced
		{
		public:
			IRecordTransform(){}
			virtual ~IRecordTransform(){}
			virtual void transform_end(CDataImportHelper* sender,long pipeid){}
			virtual TemplateRecord* transform(TemplateRecord* record,OCHeaderInfo* header) = 0;
			virtual TemplateRecord* make_line(TemplateRecord* record_line,TemplateRecord* stnod_record,TemplateRecord* ednod_record,TemplateRecord* st_record_raw){return NULL;}
			
			void setTransformNodeHeader(OCHeaderInfo* header){mTransformNodeHeader = header;}
			OCHeaderInfo* getTansformNodeHeader(){return mTransformNodeHeader;}

		protected:
			osg::ref_ptr<OCHeaderInfo> mTransformNodeHeader;
		};

		class _ModelExport CDataImportHelper
		{
		public:
			CDataImportHelper();
			~CDataImportHelper();
			enum ImportType
			{
				IMPORT_NET,
				IMPORT_SHELF
			};
			void reset();
			void setTotalTask(int num);
			int getTotalTask();
			void stop();

			void create(String net_name,int srid);

			TemplateRecordList query_source_records(IDatabase* srcDatabase,String table);

			void quick_upload_pipe(TemplateRecordList node_records,TemplateRecordList line_records,String target);
			void do_upload_shelf(String source,String target);
			long do_upload_pipe(String source,String target);
			void do_append_pipe(long pipeid,String source,String target);

			void import();

			IDatabase* getSourceDatabase();
			void setSourceDatabase(IDatabase* database);

			String getSourceConnectStr(){return mSourceConnectStr;}
			void setSourceConnectStr(String connectStr){mSourceConnectStr = connectStr;}

			IDatabase* getTargetDatabase();
			void setTartgetDatabase(IDatabase* database);

			IRecordTransform* getTransformer();
			void setTransformer(IRecordTransform* transformer);

			CDataImportListener* getListener();
			void setListener(CDataImportListener* listener);

			long make_pipe_id(String table);

			OCHeaderInfo* getTargetHeader(String table);
			OCHeaderInfo* findHeader(String table);
			void cacheHeader(String table,OCHeaderInfo* header);

			void setImportType(ImportType type){mImportType=type;}
			ImportType getImportType(){return mImportType;}

			void setSourceTable(String name){mSourceTable=name;}
			String getSourceTable(){return mSourceTable;}

			void setTargetTable(String name){mTargetTable=name;}
			String getTargetTable(){return mTargetTable;}

			void init_pipe_header(String net,int srid);
		protected:
			void onBegin();
			void onInsert();
			void onFinish();
		private:
			int									mTotalTask;
			bool								mRunning;
			ImportType					mImportType;
			String							mSourceTable;
			String							mTargetTable;
			String							mSourceConnectStr;
			osg::ref_ptr<IDatabase> mTargetDatabase;
			osg::ref_ptr<IDatabase> mSourceDatabase;

			typedef std::map<String,osg::ref_ptr<OCHeaderInfo> > HeaderMap;
			HeaderMap mCacheHeader;

			osg::ref_ptr<IRecordTransform>   mTransformer;
			CDataImportListener* mListener;
		};
	}
}

#endif



