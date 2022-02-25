#ifndef _SQLITE_CLASS_H_
#define _SQLITE_CLASS_H_

#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5

#ifdef SQLITE_TEXT
# undef SQLITE_TEXT
#define SQLITE3_TEXT     3
#else
# define SQLITE_TEXT     3
#endif

#include "OCMain/DatabaseDefine.h"

namespace OC
{
#if _WIN32 && 0
	typedef bool (WINAPI *QueryCallback) (void *para, int n_column, char **column_value, char **column_name);  
#else
	typedef bool (*QueryCallback) (void *para, int n_column, char **column_value, char **column_name);  
#endif

	typedef enum _SQLITE_DATATYPE  
	{  
		SQLITE_DATATYPE_INTEGER = SQLITE_INTEGER,  
		SQLITE_DATATYPE_FLOAT  = SQLITE_FLOAT,  
		SQLITE_DATATYPE_TEXT  = SQLITE_TEXT,  
		SQLITE_DATATYPE_BLOB = SQLITE_BLOB,  
		SQLITE_DATATYPE_NULL= SQLITE_NULL,  
	}SQLITE_DATATYPE;  


	class SQLiteDataReader  
	{  
	public:  
		SQLiteDataReader(sqlite3_stmt *pStmt);  
		~SQLiteDataReader();  
	public:  
		// 读取一行数据   
		bool Read();  
		// 关闭Reader，读取结束后调用   
		void Close();  
		// 总的列数   
		int ColumnCount(void);  
		// 获取某列的名称    
		String GetName(int nCol);  
		// 获取某列的数据类型   
		SQLITE_DATATYPE GetDataType(int nCol);  
		// 获取某列的值(字符串)   
		String GetStringValue(int nCol);  
		// 获取某列的值(整形)   
		int GetIntValue(int nCol);  
		// 获取某列的值(长整形)   
		long GetInt64Value(int nCol);  
		// 获取某列的值(浮点形)   
		double GetFloatValue(int nCol);  
		// 获取某列的值(二进制数据)   
		const uchar* GetBlobValue(int nCol, int &nLen);  
	private:  
		sqlite3_stmt *m_pStmt;  
	};  

	class SQLiteCommand  
	{  
	public:  
		SQLiteCommand(SQLite* pSqlite);  
		SQLiteCommand(SQLite* pSqlite,String lpSql);  
		~SQLiteCommand();  
	public:  
		// 设置命令   
		bool SetCommandText(String lpSql);  
		// 绑定参数（index为要绑定参数的序号，从1开始）  
		bool BindParam(int index, String szValue);

		bool BindParam(int index, const int nValue);  
		bool BindParam(int index, const long nValue);  
		bool BindParam(int index, const double dValue);  
		bool BindParam(int index, const unsigned char* blobValue, int nLen);  
		// 执行命令   
		bool Excute();  
		// 清除命令（命令不再使用时需调用该接口清除）   
		void Clear();  
	private:  
		SQLite *m_pSqlite;  
		sqlite3_stmt *m_pStmt;  
	};  

	class SQLite  
	{  
	public:  
		SQLite(void);  
		~SQLite(void);  
	public:  
		// 打开数据库   
		bool Open(String lpDbFlie);
		bool create(String lpDbFlie);

		// 关闭数据库   
		void Close();  

		// 执行非查询操作（更新或删除）   
		bool ExcuteNonQuery(String lpSql);  
		bool ExcuteNonQuery(SQLiteCommand* pCmd);  

		// 查询   
		SQLiteDataReader ExcuteQuery(String lpSql);  
		// 查询（回调方式）   
		//bool ExcuteQuery(LPCTSTR lpSql,QueryCallback pCallBack);  

		// 开始事务   
		bool BeginTransaction();  
		// 提交事务   
		bool CommitTransaction();  
		// 回滚事务   
		bool RollbackTransaction();  

		bool excute(String sql);

		// 获取上一条错误信息   
		String GetLastErrorMsg();  

		bool enableSpaitialite();
	public:  
		CORE_FRIEND_CLASS(SQLiteCommand);
		sqlite3 *m_db;  
	};  


}

#endif
