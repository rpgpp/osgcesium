#pragma once

#include "OCLayer/IDatabase.h"
#include "SQLite.h"

namespace OC
{
	class SqliteDatabase:public IDatabase
	{
	public:
		SqliteDatabase();
		~SqliteDatabase();
		virtual bool create(String url,String user = StringUtil::BLANK,String psw = StringUtil::BLANK);
		virtual bool connect(String url);
		virtual bool createTable(String name, OCHeaderInfo* headinfo);
		virtual bool existTable(String name);
		virtual TemplateRecordList query(String table, OCHeaderInfo* headinfo, CQueryInfo* filter = NULL);
		virtual bool insert(String table, TemplateRecord* record);
	private:
		SQLite mSqlite;
		bool mIsSpatialite = false;
	};
}