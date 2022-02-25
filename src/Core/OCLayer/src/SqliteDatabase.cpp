#include "SqliteDatabase.h"
#include "OCMain/DataStream.h"
#include "MetaRecord.h"
#include "IArchive.h"
#include "OCMain/SQLite.h"

#define E_SCENE_ENTITY "E_SCENE_ENTITY"
#define E_SCENE_MODEL "E_SCENE_MODEL"
#define E_SCENE_LAYER "E_SCENE_LAYER"
#define E_SCENE_METADATA "E_SCENE_METADATA"
#define E_SCENE_TEXTURE "E_SCENE_TEXTURE"
#define E_SCENE_TILE "E_SCENE_TILE"

#define CREATE_E_SCENE_ENTITY "CREATE TABLE E_SCENE_ENTITY ([ID] INTEGER PRIMARY KEY AUTOINCREMENT,[GID] INTEGER,[DNO] INTEGER,[NAME] NVARCHAR(256),[LAYERNAME] NVARCHAR(256),[MODELID] INTEGER not null,[TILEID] INTEGER,[OFFSET_X] DOUBLE(8,8) NOT NULL DEFAULT 0, [OFFSET_Y] DOUBLE(8,8) NOT NULL DEFAULT 0, [OFFSET_Z] DOUBLE(8,8) NOT NULL DEFAULT 0,[SCALE_X] DOUBLE(8,8) NOT NULL DEFAULT 1, [SCALE_Y] DOUBLE(8,8) NOT NULL DEFAULT 1, [SCALE_Z] DOUBLE(8,8) NOT NULL DEFAULT 1,[ROTATE] NVARCHAR(512),[TIMESTAMP] NVARCHAR(256));"

#define CREATE_E_SCENE_MODEL "CREATE TABLE E_SCENE_MODEL ([ID] INTEGER PRIMARY KEY AUTOINCREMENT,[NAME] VARCHAR(256),[BOUNDSPHERE] VARCHAR(512) ,[DATA] BLOB ,[TILE] INTEGER NOT NULL DEFAULT 0,[INFO] BLOB,[TIMESTAMP] NVARCHAR(256))"

#define CREATE_E_SCENE_METADATA "CREATE TABLE E_SCENE_METADATA ([ID] INTEGER PRIMARY KEY AUTOINCREMENT,[DATA] BLOB,[TIMESTAMP] NVARCHAR(256))"

#define CREATE_E_SCENE_TEXTURE "CREATE TABLE E_SCENE_TEXTURE ([ID] INTEGER PRIMARY KEY AUTOINCREMENT,[NAME] VARCHAR(256) ,[DATA] BLOB ,[TIMESTAMP] NVARCHAR(256));"

#define CREATE_E_SCENE_LAYER "CREATE TABLE [E_SCENE_LAYER] ([ID] INTEGER PRIMARY KEY AUTOINCREMENT,[NAME_DISPLAY] VARCHAR(256),[NAME_TABLE] VARCHAR(256),[VISIBLE] INTEGER NOT NULL DEFAULT 0,[DATA] BLOB,[TIMESTAMP] NVARCHAR(256))"

#define CREATE_E_SCENE_TILE "CREATE TABLE [E_SCENE_TILE] ([ID] INTEGER PRIMARY KEY AUTOINCREMENT,[ROW] INT(4) DEFAULT 0,[COL] INT(4) DEFAULT 0,[LEVEL] INT(4) DEFAULT 0,[DATA] BLOB,[TIMESTAMP] NVARCHAR(256));"


namespace OC
{
	inline bool getBlobDataToStream(SQLiteDataReader& reader, int col, StringStream& stream)
	{
		int size;
		const void* data = reader.GetBlobValue(col, size);

		if (data && size > 0)
		{
			stream.write((const char*)data, size);
			return true;
		}

		return false;
	}

	SqliteDatabase::SqliteDatabase(void)
		:mSqlite(NULL)
		, mIsTransaction(false)
	{
	}

	SqliteDatabase::~SqliteDatabase(void)
	{
		if (mSqlite && mIsTransaction)
		{
			commitTransaction();
		}

		CORE_SAFE_DELETE(mSqlite);
	}
	
	bool SqliteDatabase::create(String url,String user,String psw)
	{
		if (FileUtil::FileExist(url))
		{
			if (!FileUtil::FileDelete(url))
			{
				return false;
			}
		}

		bool ret;

		mSqlite = new SQLite;

		if (mSqlite->create(url))
		{
			ret = mSqlite->enableSpaitialite();
			ret = mSqlite->excute("SELECT InitSpatialMetadata(1)");
			ret = mSqlite->excute("SELECT InsertEpsgSrid(2437)");
			mIsSpatialite = true;

			{
				String result;
				excute(CREATE_E_SCENE_ENTITY, result);
				excute(CREATE_E_SCENE_MODEL, result);
				excute(CREATE_E_SCENE_TEXTURE, result);
				excute(CREATE_E_SCENE_LAYER, result);
				excute(CREATE_E_SCENE_TILE, result);
				excute(CREATE_E_SCENE_METADATA, result);
			}
			ret = true;
		}
		else
		{
			CORE_SAFE_DELETE(mSqlite);
			ret = false;
		}

		return ret;
	}

	void SqliteDatabase::init_meta()
	{
		osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo("ID");
		header->add(new OCFieldInfo("DATA", OC_DATATYPE_BLOB));
		header->add(new OCFieldInfo("TIMESTAMP", OC_DATATYPE_TIME));

		createTable("E_SCENE_METADATA", header);
	}

	StringVector SqliteDatabase::enumGeometryLayers()
	{
		StringVector tables;

		SQLiteDataReader pRecordset = mSqlite->ExcuteQuery("SELECT f_table_name FROM geometry_columns");
		while (pRecordset.Read())
		{
			String tablename = pRecordset.GetStringValue(0);
			tables.push_back(tablename);
		}

		return tables;
	}

	String SqliteDatabase::getDatabaseTime()
	{
		String dateStr;
		SQLiteDataReader pRecordset = mSqlite->ExcuteQuery(String("select datetime()"));
		while (pRecordset.Read())
		{
			dateStr = pRecordset.GetStringValue(0);
			break;
		}
		return dateStr;
	}

	OC::TemplateRecordList SqliteDatabase::selectTable(String table)
	{
		osg::ref_ptr<OCHeaderInfo> header;
		TemplateRecordList list;
		SQLiteDataReader pRecordset = mSqlite->ExcuteQuery("SELECT * FROM " + table);
		while (pRecordset.Read())
		{
			int col = pRecordset.ColumnCount();
			if (!header.valid())
			{
				header = new OCHeaderInfo;
				for (int i = 0; i < col; i++)
				{
					osg::ref_ptr<OCFieldInfo> fieldinfo = new OCFieldInfo;
					fieldinfo->Name = pRecordset.GetName(i);
					switch (pRecordset.GetDataType(i))
					{
					case SQLITE_DATATYPE_INTEGER:
						fieldinfo->TYPE = OC_DATATYPE_INTEGER;
						break;
					case SQLITE_DATATYPE_FLOAT:
						fieldinfo->TYPE = OC_DATATYPE_FLOAT;
						break;
					default:
						fieldinfo->TYPE = OC_DATATYPE_TEXT;
						break;
					}
					header->add(fieldinfo);
				}
			}

			osg::ref_ptr<TemplateRecord> record = new TemplateRecord(header);
			for (int i = 0; i < col; i++)
			{
				String fn = pRecordset.GetName(i);

				switch (pRecordset.GetDataType(i))
				{
				case SQLITE_DATATYPE_INTEGER:
					record->setFieldValue(fn, pRecordset.GetInt64Value(i));
					break;
				case SQLITE_DATATYPE_FLOAT:
					record->setFieldValue(fn, pRecordset.GetFloatValue(i));
					break;
				default:
					record->setFieldValue(fn, pRecordset.GetStringValue(i));
				}
			}

			list.push_back(record);
		}

		return list;
	}

	void SqliteDatabase::enumSpatialLayers()
	{
		SQLiteDataReader pRecordset = mSqlite->ExcuteQuery("SELECT f_table_name,f_geometry_column,geometry_type,srid FROM geometry_columns");
		while (pRecordset.Read())
		{
			String tablename = pRecordset.GetStringValue(0);
			String gname = pRecordset.GetStringValue(1);
			int type = pRecordset.GetIntValue(2);
			String gtype;
			if (type == 1)
			{
				gtype = "POINT";
			}
			else if (type == 2)
			{
				gtype = "LINESTRING";
			}
			int srid = pRecordset.GetIntValue(3);

			osg::ref_ptr<OCSpatialTable> spatialtable = new OCSpatialTable;
			spatialtable->setName(tablename);
			spatialtable->setGeomFieldInfo(new OCGeomFieldInfo(gname, gtype, srid));
			mSpatialTables.insert(spatialtable);
		}
	}

	void SqliteDatabase::checkSpatialLayers()
	{
		mIsSpatialite = existTable("geometry_columns");
		if (mIsSpatialite)
		{
			mSqlite->enableSpaitialite();
			enumSpatialLayers();
		}
		return;
	}

	OC::StringVector SqliteDatabase::enumNets()
	{
		StringVector tables;
		if (mIsSpatialite)
		{
			if (mSpatialTables.size() == 0)
			{
				enumSpatialLayers();
			}
			CORE_LOCK_RW_MUTEX_READ(mSpatialTables.BatchLock)

			MapIterator<RefObjectCacher<OCSpatialTable>::RefCacheMap> it = mSpatialTables.getIterator();
			while (it.hasMoreElements())
			{
				OCSpatialTable* sptable = it.getNext();
				String name = sptable->getName();
				StringVector sv = StringUtil::split(name, "_");
				if (sv.size() == 2)
				{
					String lin = sv[1];
					StringUtil::toLowerCase(lin);
					if (lin == "lin")
					{
						String net = sv[0];
						if (mSpatialTables.find(net + "_nod") != NULL)
						{
							tables.push_back(net);
						}
					}
				}
			}
		}
		else
		{
			tables = simplequery("SELECT layername FROM metainfo");
		}
		return tables;
	}

	bool SqliteDatabase::open(String url, String user, String psw)
	{
		mSqlite = new SQLite;

		if (!mSqlite->Open(url))
		{
			CORE_SAFE_DELETE(mSqlite);
			return false;
		}

		checkSpatialLayers();

		return true;
	}

	bool SqliteDatabase::update(String table, TemplateRecord* record, String whereClause)
	{
		osg::ref_ptr<OCHeaderInfo> headinfo = record->getHeadInfo();

		if (headinfo.valid())
		{
			int srid;
			String gtype;
			StringVector validFields;

			if (OCGeomFieldInfo* gfield = headinfo->getGeomFieldInfo())
			{
				osg::ref_ptr<OCSpatialTable> sptable = mSpatialTables.find(table);
				if (sptable.valid())
				{
					gtype = sptable->getGeomFieldInfo()->GeometryType;
					srid = sptable->getGeomFieldInfo()->SRID;
				}
				else
				{
					gtype = gfield->GeometryType;
					srid = gfield->SRID;
				}
			}

			String sql = "UPDATE " + table + " SET ";

			for (int i = 0; i < headinfo->getFieldNum(); i++)
			{
				OCFieldInfo* fieldinfo = headinfo->getFieldInfo(i);
				if (fieldinfo->IsPrimary)
				{
					continue;
				}

				String fn = fieldinfo->Name;
				String fieldvalue;
				if (fieldinfo->asBinaryFieldInfo())
				{
					if (record->getBlobValue(fn).isNull())
					{
						continue;
					}
				}
				else
				{
					fieldvalue = record->getFieldValue(fn);
					if (fieldvalue.empty())
					{
						continue;
					}
				}

				String fieldSql;
				switch (fieldinfo->TYPE)
				{
				case OC_DATATYPE_INTEGER:
				case OC_DATATYPE_FLOAT:
					fieldSql = "[" + fn + "]=" + fieldvalue + ",";
					break;
				case OC_DATATYPE_BLOB:
				{
					fieldSql = "[" + fn + "]=" + "?" + ",";
					validFields.push_back(fn);
				}
				break;
				case OC_DATATYPE_GEOM:
				{
					String geom;
					if (!StringUtil::startsWith(fieldvalue, gtype, false))
					{
						OCGeomFieldInfo::trim(geom);
						geom = gtype + " (" + fieldvalue + ")";
					}
					else
					{
						geom = fieldvalue;
					}
					fieldSql = "[" + fn + "]=" + "GeomFromText('" + geom + "'," + StringConverter::toString(srid) + "),";
				}
				break;
				default:
					fieldSql = "[" + fn + "]=" + "'" + fieldvalue + "'" + ",";
					break;
				}

				sql += fieldSql;
			}

			sql[sql.length() - 1] = ' ';

			if (whereClause != StringUtil::BLANK)
			{
				sql += " WHERE " + whereClause;
			}

			SQLiteCommand sqliteCommand(mSqlite);
			sqliteCommand.SetCommandText(sql);

			//BindParam after SetCommandText
			for (int i = 0; i < (int)validFields.size(); i++)
			{
				String fn = validFields[i];
				OCFieldInfo* fieldinfo = headinfo->getFieldInfo(fn);
				String fv = record->getFieldValue(fn);
				if (fieldinfo->TYPE == OC_DATATYPE_FLOAT)
				{
					sqliteCommand.BindParam(i + 1, StringConverter::parseReal(fv));
				}
				else if (fieldinfo->TYPE == OC_DATATYPE_INTEGER)
				{
					sqliteCommand.BindParam(i + 1, StringConverter::parseInt(fv));
				}
				else if (fieldinfo->TYPE == OC_DATATYPE_BLOB)
				{
					if (fieldinfo->asBinaryFieldInfo())
					{
						MemoryDataStreamPtr dataStream = record->getBlobValue(fn);
						if (!dataStream.isNull())
						{
							sqliteCommand.BindParam(i + 1, dataStream->getPtr(), dataStream->size());
						}
					}
					else
					{
						sqliteCommand.BindParam(i + 1, (uchar*)fv.c_str(), fv.length());
					}
				}
			}

			bool result = sqliteCommand.Excute();

			sqliteCommand.Clear();

			return result;
		}

		return false;
	}

	TemplateRecordList SqliteDatabase::query(String table, OCHeaderInfo* headinfo, CQueryInfo* filter)
	{
		osg::ref_ptr<CQueryInfo> query_info = filter == NULL ? new CQueryInfo : filter;

		TemplateRecordList templateRecords;

		String sqlStr = "SELECT ";
		for (int i = 0; i < headinfo->getFieldNum(); i++)
		{
			OCFieldInfo* fieldinfo = headinfo->getFieldInfo(i);

			String pre = fieldinfo->asJoinFieldInfo() ? fieldinfo->asJoinFieldInfo()->getJoinTable() : table;

			String fn = fieldinfo->Name;
			String sqlfield;
			if (fieldinfo->TYPE == OC_DATATYPE_GEOM && mIsSpatialite)
			{
				sqlfield = "ST_AsText(" + pre + "." + fn + ")";
			}
			else
			{
				sqlfield = pre + "." + "[" + fn + "]";
			}
			sqlStr += sqlfield;
			if (!fieldinfo->AliasName.empty())
			{
				sqlStr += " AS " + fieldinfo->AliasName;
			}
			sqlStr += ",";
		}

		sqlStr = sqlStr.substr(0, sqlStr.length() - 1);

		sqlStr += " FROM ";
		sqlStr += table;

		String whereOrAnd = "WHERE";

		std::map<String, String> repeat_clause;

		for (int i = 0; i < headinfo->getJoinFieldCount(); i++)
		{
			OCJoinFieldInfo* joinfield = headinfo->getJoinFieldInfo(i);
			String jTable = joinfield->getJoinTable();
			String jClause = joinfield->getJoinClause();
			if (repeat_clause[jTable] == jClause)
			{
				continue;
			}
			repeat_clause[jTable] = jClause;
			sqlStr += " LEFT OUTER JOIN " + joinfield->getJoinTable() + " ON " + joinfield->getJoinClause();
		}

		if (query_info->keyExists(CQueryInfo::NameClause))
		{
			sqlStr += " WHERE " + query_info->getStringValue(CQueryInfo::NameClause);
			whereOrAnd = "AND";
		}

		if (query_info->getRectangle(CQueryInfo::NameRectangle) != CRectangle::ZERO)
		{
			CRectangle extent = query_info->getRectangle(CQueryInfo::NameRectangle);
			String envelope = "LINESTRING (" + StringConverter::toString(extent.getMinimum()) + "," + StringConverter::toString(extent.getMaximum()) + ")";
			StringStream sstream;
			sstream << " " << whereOrAnd << " ST_EnvelopesIntersects(geom," << "ST_Envelope(ST_GeometryFromText('" << envelope << "',st_srid(geom)))" << ")";
			sqlStr += sstream.str();
		}

		if (query_info->keyExists(CQueryInfo::NamePaperRange))
		{
			CVector2 paper = query_info->getVector2Value(CQueryInfo::NamePaperRange);
			int start = paper[0] - 1;
			int count = paper[1];
			sqlStr += " limit " + StringConverter::toString(start) + "," + StringConverter::toString(count);
		}

		SQLiteDataReader pRecordset = mSqlite->ExcuteQuery(sqlStr);
		while (pRecordset.Read())
		{
			int n = headinfo->getFieldNum();
			osg::ref_ptr<TemplateRecord> templateRecord = new TemplateRecord(headinfo);
			for (int i = 0; i < n; i++)
			{
				OCFieldInfo* fieldinfo = headinfo->getFieldInfo(i);
				String fv;
				String fn = fieldinfo->Name;
				if (!fieldinfo->AliasName.empty())
				{
					fn = fieldinfo->AliasName;
				}
				switch (fieldinfo->TYPE)
				{
				case OC_DATATYPE_INTEGER:
				{
					long v = pRecordset.GetInt64Value(i);
					fv = StringConverter::toString(v);
				}
				break;
				case OC_DATATYPE_FLOAT:
				{
					double v = pRecordset.GetFloatValue(i);
					fv = StringConverter::formatDoubletoString(v);
				}
				break;
				case OC_DATATYPE_BLOB:
				{
					MemoryDataStreamPtr dataStreamPtr;
					int size;
					const void* data = pRecordset.GetBlobValue(i, size);
					if (data && size > 0)
					{
						dataStreamPtr.bind(new MemoryDataStream(size));
						dataStreamPtr->write(data, size);
					}
					templateRecord->setFieldValue(fn, dataStreamPtr);
					continue;
				}
				break;
				default:
				{
					fv = pRecordset.GetStringValue(i);
				}
				break;
				}
				templateRecord->setFieldValue(fn, fv);
			}
			templateRecords.push_back(templateRecord);

			if (filter && filter->getCallback())
			{
				filter->getCallback()->onQuery(templateRecord);
			}
		}

		return templateRecords;
	}

	OCHeaderInfo* SqliteDatabase::getHeadInfo(String table)
	{
		osg::ref_ptr<OCHeaderInfo> headinfo;
		{
			SQLiteDataReader pRecordset = mSqlite->ExcuteQuery("SELECT * FROM " + table + " limit 0,1");
			while (pRecordset.Read())
			{
				int count = pRecordset.ColumnCount();
				if (count > 0)
				{
					headinfo = new OCHeaderInfo(pRecordset.GetName(0));
					for (int i = 1; i < count; i++)
					{
						osg::ref_ptr<OCFieldInfo> fieldinfo = new OCFieldInfo;
						SQLITE_DATATYPE type = pRecordset.GetDataType(i);
						switch (type)
						{
						case SQLITE_DATATYPE_INTEGER:
							fieldinfo->TYPE = OC_DATATYPE_INTEGER;
							break;
						case SQLITE_DATATYPE_FLOAT:
							fieldinfo->TYPE = OC_DATATYPE_FLOAT;
							break;
						case SQLITE_DATATYPE_BLOB:
						{
							fieldinfo = new OCBinaryFieldInfo("");
							fieldinfo->TYPE = OC_DATATYPE_BLOB;
						}
						break;
						default:
							fieldinfo->TYPE = OC_DATATYPE_TEXT;
							break;
						}
						fieldinfo->Name = pRecordset.GetName(i);
						headinfo->add(fieldinfo);
					}
				}
				break;
			}
		}

		if (!headinfo.valid())
		{
			String sql = "PRAGMA table_info('" + table + "')";
			SQLiteDataReader pRecordset = mSqlite->ExcuteQuery(sql);
			while (pRecordset.Read())
			{
				if (!headinfo.valid())
				{
					headinfo = new OCHeaderInfo();
				}

				osg::ref_ptr<OCFieldInfo> fieldinfo = new OCFieldInfo;

				String field = pRecordset.GetStringValue(1);
				String type = pRecordset.GetStringValue(2);

				fieldinfo->Name = field;
				if (StringUtil::startsWith(type, "INTEGER", false)
					|| StringUtil::startsWith(type, "BIGINT", false)
					|| StringUtil::startsWith(type, "INT", false)
					|| StringUtil::startsWith(type, "INT64", false)
					|| StringUtil::startsWith(type, "BOOL", false))
				{
					fieldinfo->TYPE = OC_DATATYPE_INTEGER;
					fieldinfo->IsPrimary = pRecordset.GetIntValue(5) == 1;
				}
				else if (StringUtil::startsWith(type, "DOUBLE", false)
					|| StringUtil::startsWith(type, "DECIMAL", false)
					|| StringUtil::startsWith(type, "FLOAT", false))
				{
					fieldinfo->TYPE = OC_DATATYPE_FLOAT;
				}
				else if (StringUtil::startsWith(type, "BLOB", false)
					|| StringUtil::startsWith(type, "BINARY", false))
				{
					fieldinfo = new OCBinaryFieldInfo(field);
				}
				else
				{
					fieldinfo->TYPE = OC_DATATYPE_TEXT;
				}
				headinfo->add(fieldinfo);
			}
		}

		if (headinfo.valid())
		{
			if (headinfo->exist("geom"))
			{
				SQLiteDataReader pRecordset = mSqlite->ExcuteQuery("SELECT geometry_type,srid FROM geometry_columns WHERE f_geometry_column = 'geom' AND f_table_name = '" + table + "'");
				while (pRecordset.Read())
				{
					int type = pRecordset.GetIntValue(0);
					int srid = pRecordset.GetIntValue(1);
					String gtype;
					if (type == 1)
					{
						gtype = "POINT";
					}
					else if (type == 2)
					{
						gtype = "LINESTRING";
					}
					osg::ref_ptr<OCGeomFieldInfo> fieldinfo = new OCGeomFieldInfo("geom", gtype, srid);
					headinfo->replaceFieldInfo("geom", fieldinfo);
					break;
				}
			}
		}

		return headinfo.release();
	}

	StringVector SqliteDatabase::simplequery(String sqlStr)
	{
		StringVector sv;
		SQLiteDataReader pRecordset = mSqlite->ExcuteQuery(sqlStr);
		while (pRecordset.Read())
		{
			String val;
			switch (pRecordset.GetDataType(0))
			{
			case SQLITE_DATATYPE_INTEGER:
				val = StringConverter::toString(pRecordset.GetInt64Value(0));
				break;
			case SQLITE_DATATYPE_FLOAT:
				val = StringConverter::toString(pRecordset.GetFloatValue(0));
				break;
			default:
				val = pRecordset.GetStringValue(0);
				break;
			}
			sv.push_back(val);
		}
		return sv;
	}

	bool SqliteDatabase::excute(String sql,String& result)
	{
		mSqlite->excute(sql);
		return true;
	}

	bool SqliteDatabase::beginTransaction()
	{
		if (!mIsTransaction)
		{
			mSqlite->BeginTransaction();
			mIsTransaction = true;
		}
		return mIsTransaction;
	}

	bool SqliteDatabase::commitTransaction()
	{
		if (mIsTransaction)
		{
			mSqlite->CommitTransaction();
			mIsTransaction = false;
		}
		return mIsTransaction;
	}

	bool SqliteDatabase::rollbackTransaction()
	{
		return mSqlite->RollbackTransaction();
	}

	bool SqliteDatabase::query(FeatureRecord* record)
	{
		bool result = false;

		String featureURL = record->getFeatureUrl();

		if (!featureURL.empty())
		{
			osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo("ID");
			header->add(new OCFieldInfo("BOUNDSPHERE", OC_DATATYPE_TEXT));

			osg::ref_ptr<CQueryInfo> query_info = new CQueryInfo;
			query_info->setConfig(CQueryInfo::NameClause, " NAME = '" + featureURL + "'");
			TemplateRecordList list = query(E_SCENE_MODEL, header, query_info);

			if (!list.empty())
			{
				TemplateRecord* temprecord = list[0];
				record->setMID(temprecord->getID());
				record->setBoundString(temprecord->getFieldValue("BOUNDSPHERE"));
				record->setDatabase(this);
			}

			result = record->getMID() > 0;
		}

		return result;
	}

	bool SqliteDatabase::query(FeatureRecordList& list,osgDB::Options* options)
	{
		return false;
	}

	bool SqliteDatabase::update(CMetaRecord* record)
	{
		osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo("ID");
		header->add(new OCBinaryFieldInfo("DATA"));
		header->add(new OCFieldInfo("TIMESTAMP", OC_DATATYPE_TEXT));

		StringStream sstream;
		sstream << record->toXmlString();
		MemoryDataStreamPtr dataStream(new MemoryDataStream(&sstream));
		osg::ref_ptr<TemplateRecord> temprecord = new TemplateRecord(header);
		temprecord->setFieldValue("DATA", dataStream);
		temprecord->setFieldValue("TIMESTAMP", record->getTimeStamp());

		return update(E_SCENE_METADATA, temprecord ,"");
	}

	long SqliteDatabase::insert(CMetaRecord* record)
	{
		osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo("ID");
		header->add(new OCBinaryFieldInfo("DATA"));
		header->add(new OCFieldInfo("TIMESTAMP", OC_DATATYPE_TEXT));

		StringStream sstream;
		sstream << record->toXmlString();
		MemoryDataStreamPtr dataStream(new MemoryDataStream(&sstream));
		osg::ref_ptr<TemplateRecord> temprecord = new TemplateRecord(header);
		temprecord->setFieldValue("DATA", dataStream);
		temprecord->setFieldValue("TIMESTAMP", record->getTimeStamp());

		if (insert(E_SCENE_METADATA, temprecord))
		{
			return temprecord->getID();
		}

		return -1;
	}

	bool SqliteDatabase::createTable(String name, OCHeaderInfo* headinfo)
	{
		bool ret = false;

		OCFieldInfo* field1 = headinfo->getFieldInfo(0);

		String sql = "CREATE TABLE " + name + " (";

		sql += field1->Name;
		sql += " INTEGER NOT NULL PRIMARY KEY";
		sql += ")";

		ret = mSqlite->excute(sql);

		mSqlite->BeginTransaction();
		for (int i = 1; i < headinfo->getFieldNum(); i++)
		{
			OCFieldInfo* field = headinfo->getFieldInfo(i);

			String fn = A2U8(field->Name);

			StringStream sql;

			if (OCGeomFieldInfo* geomfieldinfo = field->asGeomFieldInfo())
			{
				geomfieldinfo->SRID;
				sql << "SELECT AddGeometryColumn('" << name << "','" << fn << "',"
					<< StringConverter::toString(geomfieldinfo->SRID) << ",'"
					<< geomfieldinfo->GeometryType << "','XY')";
			}
			else
			{
				sql << "ALTER TABLE " + name + " ADD COLUMN [" + fn + "] ";
				switch (field->TYPE)
				{
				case OC_DATATYPE_INTEGER:
					sql << "INTEGER";
					break;
				case OC_DATATYPE_BLOB:
					sql << "BLOB";
					break;
				case OC_DATATYPE_FLOAT:
					sql << "DOUBLE";
					//sql << "DECIMAL(";
					//sql << StringConverter::toString(field->Length) + "," + StringConverter::toString(field->Precision) + ")";
					break;
				case OC_DATATYPE_TIME:
					sql << "TIMESTAMP";
					break;
				default:
					sql << "TEXT";
					break;
				}
			}

			ret = mSqlite->excute(sql.str());
		}

		mSqlite->CommitTransaction();

		if (OCGeomFieldInfo* gfield = headinfo->getGeomFieldInfo())
		{
			if (ret)
			{
				osg::ref_ptr<OCSpatialTable> sptable = new OCSpatialTable;
				sptable->setName(name);
				sptable->setGeomFieldInfo(gfield);
				mSpatialTables.insert(sptable);
			}
		}

		//ret = mSqlite->excute("SELECT CreateSpatialIndex('" + name + "', 'geom')");

		return ret;
	}

	bool SqliteDatabase::existTable(String table)
	{
		String sql = "select * from sqlite_master where type = 'table' and name = '" + table + "'";
		SQLiteDataReader pRecordset = mSqlite->ExcuteQuery(sql);
		while (pRecordset.Read())
		{
			return true;
		}
		return false;
	}

	bool SqliteDatabase::query(CMetaRecord* record)
	{
		osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo("ID");
		header->add(new OCBinaryFieldInfo("DATA"));
		header->add(new OCFieldInfo("TIMESTAMP", OC_DATATYPE_TEXT));

		TemplateRecordList list = query(E_SCENE_METADATA, header);
		if (!list.empty())
		{
			TemplateRecord* temprecord = list[0];
			MemoryDataStreamPtr dataStream = temprecord->getBlobValue("DATA");
			if (!dataStream.isNull())
			{
				StringStream sstream;
				sstream.write((const char*)dataStream->getPtr(), dataStream->size());
				record->parseXmlString(sstream.str());
			}
			record->setTimeStamp(temprecord->getFieldValue("TIMESTAMP"));
			record->setDatabase(this);
			return true;
		}

		return false;
	}

	long SqliteDatabase::insert(FeatureRecord* record,OperType ot)
	{
		bool ret = false;

		if (ot == OP_ENTITY)
		{
			if (!mEntityHeader.valid())
			{
				mEntityHeader = getHeadInfo(E_SCENE_ENTITY);
			}

			CVector3 offset = record->getOffset();
			CVector3 scale = record->getScale();
			osg::ref_ptr<TemplateRecord> temprecord = new TemplateRecord(mEntityHeader);
			temprecord->setFieldValue("NAME", record->getFeatureName());
			temprecord->setFieldValue("MODELID", record->getMID());
			temprecord->setFieldValue("OFFSET_X", offset.x);
			temprecord->setFieldValue("OFFSET_Y", offset.y);
			temprecord->setFieldValue("OFFSET_Z", offset.z);
			temprecord->setFieldValue("SCALE_X", scale.x);
			temprecord->setFieldValue("SCALE_Y", scale.y);
			temprecord->setFieldValue("SCALE_Z", scale.z);
			temprecord->setFieldValue("GID", record->getGID());
			temprecord->setFieldValue("DNO", record->getDNO());
			temprecord->setFieldValue("LAYERNAME", record->getTable());
			temprecord->setFieldValue("ROTATE", record->getRotateString());
			temprecord->setFieldValue("TIMESTAMP", record->getTime());
			temprecord->setFieldValue("TILEID", record->getTileID());
			insert(E_SCENE_ENTITY, temprecord);
			return temprecord->getID();
		}
		else if (ot == OP_MODEL)
		{
			String modelName = osgDB::convertStringFromCurrentCodePageToUTF8(record->getFeatureUrl());

			if (!mModelHeader.valid())
			{
				mModelHeader = getHeadInfo(E_SCENE_MODEL);
			}

			osg::ref_ptr<TemplateRecord> temprecord = new TemplateRecord(mModelHeader);
			temprecord->setFieldValue("NAME", record->getFeatureUrl());
			temprecord->setFieldValue("BOUNDSPHERE", record->getBoundString());
			temprecord->setFieldValue("DATA", record->getData());
			temprecord->setFieldValue("TILE", (int)record->isTileModel() ? 1 : 0);
			insert(E_SCENE_MODEL, temprecord);

			return temprecord->getID();
		}

		return -1;
	}

	bool SqliteDatabase::insertTexture(String name,MemoryDataStreamPtr dataStream)
	{
		osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo();
		header->add(new OCFieldInfo("NAME", OC_DATATYPE_TEXT));
		header->add(new OCBinaryFieldInfo("DATA"));
		osg::ref_ptr<TemplateRecord> temprecord = new TemplateRecord(header);
		temprecord->setFieldValue("NAME", name);
		temprecord->setFieldValue("DATA", dataStream);
		insert(E_SCENE_TEXTURE, temprecord);
		return temprecord->getID();
	}

	bool SqliteDatabase::update(FeatureRecord* record)
	{
		bool ret = false;

		int ID = record->getID();
		if (ID < 0)
		{
			return ret;
		}

		if (!mEntityHeader.valid())
		{
			mEntityHeader = getHeadInfo(E_SCENE_ENTITY);
		}

		CVector3 offset = record->getOffset();
		CVector3 scale = record->getScale();

		osg::ref_ptr<TemplateRecord> temprecord = new TemplateRecord(mEntityHeader);
		temprecord->setFieldValue("NAME", record->getFeatureName());
		temprecord->setFieldValue("LAYERNAME", record->getTable());
		temprecord->setFieldValue("MODELID", record->getMID());
		temprecord->setFieldValue("OFFSET_X", offset.x);
		temprecord->setFieldValue("OFFSET_Y", offset.y);
		temprecord->setFieldValue("OFFSET_Z", offset.z);
		temprecord->setFieldValue("SCALE_X", scale.x);
		temprecord->setFieldValue("SCALE_Y", scale.y);
		temprecord->setFieldValue("SCALE_Z", scale.z);
		temprecord->setFieldValue("ROTATE", record->getRotateString());

		ret = update(E_SCENE_ENTITY, temprecord, String("ID = ") + StringConverter::toString(ID));

		return ret;
	}

	bool SqliteDatabase::getModelStream(String featureURL, StringStream& stream)
	{
		bool result = false;

		if (!featureURL.empty())
		{
			String sql = "select DATA from E_SCENE_MODEL where NAME = '" + featureURL + "'";
			SQLiteDataReader sqlReader = mSqlite->ExcuteQuery(sql);
			while (sqlReader.Read())
			{
				result = getBlobDataToStream(sqlReader, 0, stream);
				break;
			}
		}

		return result;
	}

	bool SqliteDatabase::getTextureStream(String texture, StringStream& stream)
	{
		bool result = false;

		if (!texture.empty())
		{
			String simple = osgDB::getSimpleFileName(texture);
			String whereClause = "NAME = '" + texture + "' COLLATE NOCASE";
			if (simple != texture)
			{
				whereClause += " OR NAME = '" + simple + "' COLLATE NOCASE";
			}

			String sql = "SELECT DATA FROM E_SCENE_TEXTURE WHERE " + whereClause;

			SQLiteDataReader sqlReader = mSqlite->ExcuteQuery(sql);
			while (sqlReader.Read())
			{
				result = getBlobDataToStream(sqlReader, 0, stream);
				break;
			}
		}

		return result;
	}

	MemoryDataStreamPtr SqliteDatabase::readTextureData(String texture)
	{
		MemoryDataStreamPtr dataStream;

		String simple = osgDB::getSimpleFileName(texture);

		osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo;
		header->add(new OCBinaryFieldInfo("DATA"));

		String clause = "NAME = '" + texture + "' COLLATE NOCASE";
		if (simple != texture)
		{
			clause += " OR NAME = '" + simple + "' COLLATE NOCASE";
		}

		osg::ref_ptr<CQueryInfo> query_info = new CQueryInfo;
		query_info->setConfig(CQueryInfo::NameClause, clause);
		TemplateRecordList list = query(E_SCENE_TEXTURE, header, query_info);
		if (!list.empty())
		{
			dataStream = list[0]->getBlobValue("DATA");
		}

		return dataStream;
	}

	bool SqliteDatabase::connect(String connectStr)
	{
		mSqlite = new SQLite;

		if (!mSqlite->Open(connectStr))
		{
			CORE_SAFE_DELETE(mSqlite);
			return false;
		}

		checkSpatialLayers();

		return true;
	}

	bool SqliteDatabase::insert(String tablename, TemplateRecord* record)
	{
		OCHeaderInfo* headinfo = record->getHeadInfo();
		if (headinfo == NULL)
		{
			return false;
		}

		String sql = "INSERT INTO " + tablename + "(";
		String valSql = " VALUES(";

		int srid;
		String gtype;

		if (OCGeomFieldInfo* gfield = headinfo->getGeomFieldInfo())
		{
			osg::ref_ptr<OCSpatialTable> sptable = mSpatialTables.find(tablename);
			if (sptable.valid())
			{
				gtype = sptable->getGeomFieldInfo()->GeometryType;
				srid = sptable->getGeomFieldInfo()->SRID;
			}
			else
			{
				gtype = gfield->GeometryType;
				srid = gfield->SRID;
			}
		}

		SQLiteCommand sqliteCommand(mSqlite);

		StringVector validFields;

		for (int i = 0; i < headinfo->getFieldNum(); i++)
		{
			OCFieldInfo* fieldinfo = headinfo->getFieldInfo(i);
			String fn = fieldinfo->Name;

			if (fieldinfo->IsPrimary)
			{
				continue;
			}

			String fieldvalue;

			if (fieldinfo->asBinaryFieldInfo())
			{
				if (record->getBlobValue(fn).isNull())
				{
					continue;
				}
			}
			else
			{
				fieldvalue = record->getFieldValue(fn);
				if (fieldvalue.empty())
				{
					continue;
				}
			}

			if (fieldinfo->asGeomFieldInfo())
			{
				String geom;
				if (!StringUtil::startsWith(fieldvalue, gtype, false))
				{
					OCGeomFieldInfo::trim(geom);
					geom = gtype + " (" + fieldvalue + ")";
				}
				else
				{
					geom = fieldvalue;
				}

				geom = "GeomFromText('" + geom + "'," + StringConverter::toString(srid) + ")";

				valSql += geom + ",";
			}
			else if (fieldinfo->TYPE == OC_DATATYPE_INTEGER)
			{
				valSql += "?,";
				validFields.push_back(fn);
			}
			else if (fieldinfo->TYPE == OC_DATATYPE_FLOAT)
			{
				valSql += "?,";
				validFields.push_back(fn);
			}
			else if (fieldinfo->TYPE == OC_DATATYPE_BLOB)
			{
				valSql += "?,";
				validFields.push_back(fn);
			}
			else
			{
				valSql += "'" + fieldvalue + "',";
			}

			if (!fieldinfo->asGeomFieldInfo())
			{
				fn = "[" + fn + "]";
			}

			sql += fn + ",";
		}

		sql[sql.length() - 1] = ')';
		valSql[valSql.length() - 1] = ')';

		sql += valSql;

		sqliteCommand.SetCommandText(sql);


		//BindParam after SetCommandText
		for (int i = 0; i < (int)validFields.size(); i++)
		{
			String fn = validFields[i];
			OCFieldInfo* fieldinfo = headinfo->getFieldInfo(fn);
			String fv = record->getFieldValue(fn);
			if (fieldinfo->TYPE == OC_DATATYPE_FLOAT)
			{
				sqliteCommand.BindParam(i + 1, StringConverter::parseReal(fv));
			}
			else if (fieldinfo->TYPE == OC_DATATYPE_INTEGER)
			{
				sqliteCommand.BindParam(i + 1, StringConverter::parseInt(fv));
			}
			else if (fieldinfo->TYPE == OC_DATATYPE_BLOB)
			{
				if (fieldinfo->asBinaryFieldInfo())
				{
					MemoryDataStreamPtr dataStream = record->getBlobValue(fn);
					if (!dataStream.isNull())
					{
						sqliteCommand.BindParam(i + 1, dataStream->getPtr(), dataStream->size());
					}
				}
				else
				{
					sqliteCommand.BindParam(i + 1, (uchar*)fv.c_str(), fv.length());
				}
			}
		}


		bool result = sqliteCommand.Excute();

		sqliteCommand.Clear();

		if (headinfo->getPrimaryFiledInfo())
		{
			SQLiteDataReader sqlReader = mSqlite->ExcuteQuery("select last_insert_rowid()");

			while (sqlReader.Read())
			{
				record->setPrimaryValue(StringConverter::toString(sqlReader.GetInt64Value(0)));
				break;
			}
		}

		return result;
	}

}
