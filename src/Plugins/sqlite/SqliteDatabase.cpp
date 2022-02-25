#include "SqliteDatabase.h"

namespace OC
{
	SqliteDatabase::SqliteDatabase()
	{
			
	}

	SqliteDatabase::~SqliteDatabase()
	{

	}

	bool SqliteDatabase::create(String url, String user, String psw)
	{
		if (FileUtil::FileExist(url))
		{
			FileUtil::FileDelete(url);
		}
		return mSqlite.create(url);
	}

	bool SqliteDatabase::connect(String url)
	{
		return mSqlite.Open(url);
	}

	bool SqliteDatabase::createTable(String name, OCHeaderInfo* headinfo)
	{
		bool ret = false;

		OCFieldInfo* field1 = headinfo->getFieldInfo(0);

		String sql = "CREATE TABLE " + name + " (";

		sql += field1->Name;
		sql += " INTEGER NOT NULL PRIMARY KEY";
		sql += ")";

		ret = mSqlite.excute(sql);

		mSqlite.BeginTransaction();
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

			ret = mSqlite.excute(sql.str());
		}

		mSqlite.CommitTransaction();

		if (OCGeomFieldInfo* gfield = headinfo->getGeomFieldInfo())
		{
			if (ret)
			{
				osg::ref_ptr<OCSpatialTable> sptable = new OCSpatialTable;
				sptable->setName(name);
				sptable->setGeomFieldInfo(gfield);
				//mSpatialTables.insert(sptable);
			}
		}

		//ret = mSqlite.excute("SELECT CreateSpatialIndex('" + name + "', 'geom')");

		return ret;
	}

	bool SqliteDatabase::existTable(String table)
	{
		String sql = "select * from sqlite_master where type = 'table' and name = '" + table + "'";
		SQLiteDataReader pRecordset = mSqlite.ExcuteQuery(sql);
		while (pRecordset.Read())
		{
			return true;
		}
		return false;
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

		int srid = 4326;
		String gtype;

		if (OCGeomFieldInfo* gfield = headinfo->getGeomFieldInfo())
		{
			//osg::ref_ptr<OCSpatialTable> sptable = mSpatialTables.find(tablename);
			//if (sptable.valid())
			//{
			//	gtype = sptable->getGeomFieldInfo()->GeometryType;
			//	srid = sptable->getGeomFieldInfo()->SRID;
			//}
			//else
			//{
			//	gtype = gfield->GeometryType;
			//	srid = gfield->SRID;
			//}
		}

		SQLiteCommand sqliteCommand(&mSqlite);

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
			SQLiteDataReader sqlReader = mSqlite.ExcuteQuery("select last_insert_rowid()");

			while (sqlReader.Read())
			{
				record->setPrimaryValue(StringConverter::toString(sqlReader.GetInt64Value(0)));
				break;
			}
		}

		return result;
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

		SQLiteDataReader pRecordset = mSqlite.ExcuteQuery(sqlStr);
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
};

