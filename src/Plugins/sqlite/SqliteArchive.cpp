#include "SqliteArchive.h"
#include "SqliteDatabase.h"
#include "OCMain/JsonHelper.h"

using namespace OC;
 
SqliteArchive::SqliteArchive()
{
	supportsExtension("sqlite","sqlite data format");
}

SqliteArchive::~SqliteArchive()
{
}

bool SqliteArchive::open(String filename,ArchiveStatus status,const osgDB::ReaderWriter::Options* options)
{
	osg::ref_ptr<SqliteDatabase> db = new SqliteDatabase;

	StringMap sm = JsonHelper::parseStyleString(filename);
	StringMap::iterator it = sm.find("location");
	String file = it == sm.end() ? filename : it->second;

	if(status == CREATE)
	{
		if (db->create(file))
		{
			mDatabase = db;
		}
	}
	else
	{
		if (db->connect(file))
		{
			mDatabase = db;
		}
	}

	return mDatabase.valid();
}

osgDB::ReaderWriter::ReadResult SqliteArchive::readNode(const String& fileName, const osgDB::ReaderWriter::Options* options) const
{
	if (mDatabase.valid())
	{
		String ext = FileUtil::getLowerCaseFileExtension(fileName);
		if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(ext))
		{
			osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo("ID");
			header->add(new OCFieldInfo("NAME", OC_DATATYPE_TEXT));
			header->add(new OCBinaryFieldInfo("DATA"));
			osg::ref_ptr<CQueryInfo> query_info = new CQueryInfo();
			query_info->setConfig(CQueryInfo::NameClause, "NAME = " + fileName);
			TemplateRecordList list = mDatabase->query("OC_DATA", header, query_info);
			if (!list.empty())
			{
				TemplateRecord* record = list[0];
				MemoryDataStreamPtr dataStream = record->getBlobValue("DATA");
				StringStream sstream;
				sstream.write((const char*)dataStream->getPtr(), dataStream->size());
				return rw->readNode(sstream,options);
			}
		}
	}

	return ReadResult::FILE_NOT_HANDLED;
}

osgDB::ReaderWriter::ReadResult SqliteArchive::openArchive(const String& fileName,ReaderWriter::ArchiveStatus status, unsigned int dep, const Options* options) const
{
	String ext = osgDB::getLowerCaseFileExtension(fileName);
	if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

	String connectStr = fileName;

	if (options)
	{
		String optStr = options->getOptionString();
		if (!optStr.empty())
		{
			connectStr = optStr;
		}
	}

	osg::ref_ptr<SqliteArchive> archive = new SqliteArchive;
	if (archive->open(connectStr,status,options))
	{
		return archive.get();
	}

	return ReadResult::FILE_NOT_FOUND; 
}

REGISTER_OSGPLUGIN(sqlite, SqliteArchive)
