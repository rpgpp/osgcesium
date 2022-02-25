#include "SQLite.h"
#include <sqlite3.h>

#define UNICODE

namespace OC
{
	SQLite::SQLite(void):  
	m_db(NULL)  
	{  
	}  

	SQLite::~SQLite(void)  
	{  
		Close();  
	}  

	bool SQLite::enableSpaitialite()
	{ 
		if (m_db)
		{
			sqlite3_enable_load_extension(m_db,1);
			return SQLITE_OK == sqlite3_exec(m_db, "SELECT load_extension('spatialite.dll','sqlite3_spatialite_init')", 0,0,0);
		}

		return false;
	}  

	bool SQLite::create(String lpDbFlie)  
	{  
		if(lpDbFlie.empty())  
		{  
			return false;  
		}  
		if(sqlite3_open_v2(osgDB::convertStringFromCurrentCodePageToUTF8(lpDbFlie).c_str(),&m_db,SQLITE_OPEN_CREATE|SQLITE_OPEN_READWRITE|SQLITE_OPEN_NOMUTEX,NULL) != SQLITE_OK)  
		{  
			//sqlite3_exec(m_db, "PRAGMA synchronous = OFF; ", 0,0,0);
			return false;  
		}  
		return true;  
	}  

	bool SQLite::Open(String lpDbFlie)  
	{  
		if(lpDbFlie.empty())  
		{  
			return false;  
		}  
#if defined( UNICODE) && defined( _WIN32 )
		if(sqlite3_open_v2(osgDB::convertStringFromCurrentCodePageToUTF8(lpDbFlie).c_str(),&m_db,SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX,NULL) != SQLITE_OK)  
#else   
		if(sqlite3_open(lpDbFlie.c_str(),&m_db) != SQLITE_OK)  
#endif   
		{  
			return false;  
		}  
		return true;  
	}  

	void SQLite::Close()  
	{  
		if(m_db)  
		{  
			sqlite3_close(m_db);  
			m_db = NULL;  
		}  
	}  

	bool SQLite::ExcuteNonQuery(String lpSql)  
	{  
		if(lpSql.empty())  
		{  
			return false;  
		}  
		sqlite3_stmt* stmt;    
#if defined( UNICODE) && defined( _WIN32 )
		if(sqlite3_prepare16_v2(m_db, A2U(lpSql).c_str(), -1, &stmt, NULL) != SQLITE_OK)  
#else   
		if(sqlite3_prepare_v2(m_db, lpSql.c_str(), -1, &stmt, NULL) != SQLITE_OK)  
#endif   
		{   
			return false;    
		}    
		sqlite3_step(stmt);  

		return (sqlite3_finalize(stmt) == SQLITE_OK) ? true : false ;  
	}  

	bool SQLite::ExcuteNonQuery(SQLiteCommand* pCmd)  
	{  
		if(pCmd == NULL)  
		{  
			return false;  
		}  
		return pCmd->Excute();  
	}  

	/*
	// 查询(回调方式)   
	bool SQLite::ExcuteQuery(LPCTSTR lpSql,QueryCallback pCallBack)  
	{  
		if(lpSql == NULL || pCallBack == NULL)  
		{  
			return false;  
		}  
		char *errmsg = NULL;  
#ifdef  UNICODE    
		const char *szSql = WcharToUtf8(lpSql);  
		if(sqlite3_exec(m_db, szSql, pCallBack, NULL, &errmsg) != SQLITE_OK)  
		{  
			delete[] szSql;  
			return false;  
		}   
		delete[] szSql;  
#else   
		if(sqlite3_exec(m_db, lpSql, pCallBack, NULL, &errmsg) != SQLITE_OK)  
		{  
			return false;  
		}   
#endif   
		return true;  
	}  */

	// 查询   
	SQLiteDataReader SQLite::ExcuteQuery(String lpSql)
	{
		sqlite3_stmt* stmt;   
#if defined( UNICODE) && defined( _WIN32 )
		if(sqlite3_prepare16_v2(m_db, A2U(lpSql).c_str(), -1, &stmt, NULL) != SQLITE_OK)  
#else   
		if(sqlite3_prepare_v2(m_db, lpSql.c_str(), -1, &stmt, NULL) != SQLITE_OK)  
#endif   
		{   
			return SQLiteDataReader(NULL);    
		}    
		return SQLiteDataReader(stmt);  
	}

	// 开始事务   
	bool SQLite::BeginTransaction()  
	{  
		char * errmsg = NULL;  
		if(sqlite3_exec(m_db,"begin;",NULL,NULL,&errmsg) != SQLITE_OK)  
		{  
			sqlite3_free(errmsg);
			return false;  
		}   
		return true;  
	}  

	// 提交事务   
	bool SQLite::CommitTransaction()  
	{  
		char * errmsg = NULL;  
		if(sqlite3_exec(m_db,"commit;",NULL,NULL,&errmsg) != SQLITE_OK)  
		{  
			return false;  
		}   
		return true;  
	}  

	// 回滚事务   
	bool SQLite::RollbackTransaction()  
	{  
		char * errmsg = NULL;  
		if(sqlite3_exec(m_db,"ROLLBACK  TRANSACTION;",NULL,NULL,&errmsg) != SQLITE_OK)  
		{  
			return false;  
		}   
		return true;  
	}  

	bool SQLite::excute(String sql)
	{  
		char * errmsg = NULL;  

		if(sqlite3_exec(m_db,sql.c_str(),NULL,NULL,&errmsg) != SQLITE_OK)  
		{  
			return false;  
		}   
		return true;  
	}  

	// 获取上一条错误信息   
	String SQLite::GetLastErrorMsg()  
	{  
#ifdef UNICODE    
		return String((char*)sqlite3_errmsg16(m_db));  
#else   
		return String((char*)sqlite3_errmsg(m_db));  
#endif   
	}  



	SQLiteDataReader::SQLiteDataReader(sqlite3_stmt *pStmt):  
	m_pStmt(pStmt)  
	{  

	}  

	SQLiteDataReader::~SQLiteDataReader()  
	{  
		Close();  
	}  

	// 读取一行数据   
	bool SQLiteDataReader::Read()  
	{  
		if(m_pStmt == NULL)  
		{  
			return false;  
		}  
		if(sqlite3_step(m_pStmt) != SQLITE_ROW)  
		{  
			return false;  
		}  
		return true;  
	}  

	// 关闭Reader，读取结束后调用   
	void SQLiteDataReader::Close()  
	{  
		if(m_pStmt)  
		{  
			sqlite3_finalize(m_pStmt);  
			m_pStmt = NULL;  
		}  
	}  

	// 总的列数   
	int SQLiteDataReader::ColumnCount(void)  
	{  
		return sqlite3_column_count(m_pStmt);  
	}  

	// 获取某列的名称    
	String SQLiteDataReader::GetName(int nCol)  
	{  
#if defined( UNICODE) && defined( _WIN32 )
		wchar_t* data = (wchar_t*)sqlite3_column_name16( m_pStmt, nCol );
		if (data != NULL)
		{
			return U2A(data);
		}
		return StringUtil::BLANK;
#else   
		return String(sqlite3_column_name(m_pStmt, nCol));  
#endif   
	}  

	// 获取某列的数据类型   
	SQLITE_DATATYPE SQLiteDataReader::GetDataType(int nCol)  
	{  
		return (SQLITE_DATATYPE)sqlite3_column_type(m_pStmt, nCol);  
	}  

	// 获取某列的值(字符串)   
	String SQLiteDataReader::GetStringValue(int nCol)  
	{  
		wchar_t* data = (wchar_t*)sqlite3_column_text16( m_pStmt, nCol );

		if (data != NULL)
		{
			return U2A(data);
		}

		return StringUtil::BLANK;
	}  

	// 获取某列的值(整形)   
	int SQLiteDataReader::GetIntValue(int nCol)  
	{  
		return sqlite3_column_int(m_pStmt, nCol);  
	}  

	// 获取某列的值(长整形)   
	long SQLiteDataReader::GetInt64Value(int nCol)  
	{  
		return (long)sqlite3_column_int64(m_pStmt, nCol);  
	}  

	// 获取某列的值(浮点形)   
	double SQLiteDataReader::GetFloatValue(int nCol)  
	{  
		return sqlite3_column_double(m_pStmt, nCol);  
	}  

	// 获取某列的值(二进制数据)   
	const uchar* SQLiteDataReader::GetBlobValue(int nCol, int &nLen)  
	{  
		nLen = sqlite3_column_bytes(m_pStmt, nCol);  
		return (const uchar*)sqlite3_column_blob(m_pStmt, nCol);  
	}  

	SQLiteCommand::SQLiteCommand(SQLite* pSqlite):  
	m_pSqlite(pSqlite),  
		m_pStmt(NULL)  
	{  
	}  

	SQLiteCommand::SQLiteCommand(SQLite* pSqlite,String lpSql):  
	m_pSqlite(pSqlite),  
		m_pStmt(NULL)  
	{  
		SetCommandText(lpSql);  
	}  

	SQLiteCommand::~SQLiteCommand()  
	{  

	}  

	bool SQLiteCommand::SetCommandText(String lpSql)  
	{  
#if defined( UNICODE) && defined( _WIN32 )
		if(sqlite3_prepare16_v2(m_pSqlite->m_db, A2U(lpSql).c_str(), -1, &m_pStmt, NULL) != SQLITE_OK)  
#else   
		if(sqlite3_prepare_v2(m_pSqlite->m_db, lpSql.c_str(), -1, &m_pStmt, NULL) != SQLITE_OK)  
#endif   
		{   
			return false;    
		}    
		return true;  
	}  

	bool SQLiteCommand::BindParam(int index, String szValue)  
	{
		if(sqlite3_bind_text(m_pStmt, index, szValue.c_str(),-1, SQLITE_TRANSIENT) != SQLITE_OK)  
		{
			return true;
		}
		return false;
	}

	bool SQLiteCommand::BindParam(int index, const int nValue)  
	{  
		if(sqlite3_bind_int(m_pStmt, index, nValue) != SQLITE_OK)  
		{  
			return false;  
		}  
		return true;  
	}  

	bool SQLiteCommand::BindParam(int index, const long nValue)  
	{  
		if(sqlite3_bind_int64(m_pStmt, index, nValue) != SQLITE_OK)  
		{  
			return false;  
		}  
		return true;  
	}  

	bool SQLiteCommand::BindParam(int index, const double dValue)  
	{  
		if(sqlite3_bind_double(m_pStmt, index, dValue) != SQLITE_OK)  
		{  
			return false;  
		}  
		return true;  
	}  

	bool SQLiteCommand::BindParam(int index, const unsigned char* blobBuf, int nLen)  
	{  
		if(sqlite3_bind_blob(m_pStmt, index, blobBuf,nLen,NULL) != SQLITE_OK)  
		{  
			return false;  
		}  
		return true;  
	}  

	bool SQLiteCommand::Excute()  
	{  
		sqlite3_step(m_pStmt);  

		return (sqlite3_reset(m_pStmt) == SQLITE_OK) ? true : false ;  
	}  

	void SQLiteCommand::Clear()  
	{  
		if(m_pStmt)  
		{  
			sqlite3_finalize(m_pStmt);  
		}  
	}  

}
