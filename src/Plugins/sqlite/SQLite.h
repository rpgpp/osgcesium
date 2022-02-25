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
		// ��ȡһ������   
		bool Read();  
		// �ر�Reader����ȡ���������   
		void Close();  
		// �ܵ�����   
		int ColumnCount(void);  
		// ��ȡĳ�е�����    
		String GetName(int nCol);  
		// ��ȡĳ�е���������   
		SQLITE_DATATYPE GetDataType(int nCol);  
		// ��ȡĳ�е�ֵ(�ַ���)   
		String GetStringValue(int nCol);  
		// ��ȡĳ�е�ֵ(����)   
		int GetIntValue(int nCol);  
		// ��ȡĳ�е�ֵ(������)   
		long GetInt64Value(int nCol);  
		// ��ȡĳ�е�ֵ(������)   
		double GetFloatValue(int nCol);  
		// ��ȡĳ�е�ֵ(����������)   
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
		// ��������   
		bool SetCommandText(String lpSql);  
		// �󶨲�����indexΪҪ�󶨲�������ţ���1��ʼ��  
		bool BindParam(int index, String szValue);

		bool BindParam(int index, const int nValue);  
		bool BindParam(int index, const long nValue);  
		bool BindParam(int index, const double dValue);  
		bool BindParam(int index, const unsigned char* blobValue, int nLen);  
		// ִ������   
		bool Excute();  
		// �����������ʹ��ʱ����øýӿ������   
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
		// �����ݿ�   
		bool Open(String lpDbFlie);
		bool create(String lpDbFlie);

		// �ر����ݿ�   
		void Close();  

		// ִ�зǲ�ѯ���������»�ɾ����   
		bool ExcuteNonQuery(String lpSql);  
		bool ExcuteNonQuery(SQLiteCommand* pCmd);  

		// ��ѯ   
		SQLiteDataReader ExcuteQuery(String lpSql);  
		// ��ѯ���ص���ʽ��   
		//bool ExcuteQuery(LPCTSTR lpSql,QueryCallback pCallBack);  

		// ��ʼ����   
		bool BeginTransaction();  
		// �ύ����   
		bool CommitTransaction();  
		// �ع�����   
		bool RollbackTransaction();  

		bool excute(String sql);

		// ��ȡ��һ��������Ϣ   
		String GetLastErrorMsg();  

		bool enableSpaitialite();
	public:  
		CORE_FRIEND_CLASS(SQLiteCommand);
		sqlite3 *m_db;  
	};  


}

#endif
