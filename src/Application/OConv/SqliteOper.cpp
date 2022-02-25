#include "SqliteOper.h"
#include "OCLayer/IArchive.h"
#include "OCLayer/ReadFileCallback.h"
#include "OCLayer/OCResource.h"
#include "OCMain/Environment.h"

using namespace OC;

IDatabase* openDatabase(String conectStr, osgDB::ReaderWriter::ArchiveStatus status)
{
	osg::ref_ptr<IDatabase> database;
	osg::ref_ptr<IArchive> mArchive = static_cast<IArchive*>(osgDB::Registry::instance()->openArchive(conectStr, status, 0, NULL).takeArchive());
	if (mArchive.valid())
	{
		database = mArchive->getDatabase();
	}

	mArchive = NULL;

	return database.release();

	
}

void createSqlite()
{
	osgDB::Registry::instance()->setReadFileCallback(new OCReadFileCallback);
	String filename1 = Singleton(Environment).getAppDir() + "data/resource.db";
	String filename2 = Singleton(Environment).getTempDir() + "resource.db";
	String conectStr = "driver:sqlite;location:";
	osg::ref_ptr<IDatabase> database1 = openDatabase(conectStr + filename1, osgDB::ReaderWriter::READ);
	osg::ref_ptr<IDatabase> database2 = openDatabase(conectStr + filename2, osgDB::ReaderWriter::CREATE);

	osg::ref_ptr<OC::OCHeaderInfo> header2 = new OCHeaderInfo("ID");
	header2->add(new OCFieldInfo("NAME", OC_DATATYPE_TEXT));
	header2->add(new OCBinaryFieldInfo("DATA"));
	header2->add(new OCFieldInfo("DESCRIPTION", OC_DATATYPE_TEXT));
	database2->createTable("OC_DATA", header2);

	{
		osg::ref_ptr<OCHeaderInfo> header1 = new OCHeaderInfo("ID");
		header1->add(new OCFieldInfo("NAME", OC_DATATYPE_TEXT));
		header1->add(new OCBinaryFieldInfo("DATA"));
		header1->add(new OCFieldInfo("BOUNDSPHERE", OC_DATATYPE_TEXT));
		TemplateRecordList list = database1->query("E_SCENE_MODEL", header1);
		for (TemplateRecordList::iterator it = list.begin();
			it != list.end(); it++)
		{
			TemplateRecord* record = it->get();
			MemoryDataStreamPtr dataStream = record->getBlobValue("DATA");
			if (!dataStream.isNull())
			{
				String name = record->getFieldValue("NAME");
				String bsf = record->getFieldValue("BOUNDSPHERE");
				CVector4 v = StringConverter::parseVector4(bsf);
				if (Math::RealEqual(v.x, 0.0,1e-3))
					v.x = 0.0;
				if (Math::RealEqual(v.y, 0.0, 1e-3))
					v.y = 0.0;
				if (Math::RealEqual(v.z, 0.0, 1e-3))
					v.z = 0.0;
				if (Math::RealEqual(v.w, 0.0, 1e-3))
					v.w = 0.0;
				String desc = StringConverter::formatDoubletoString(v.x, 3) + " " + 
					StringConverter::formatDoubletoString(v.y,3) + " " +
					StringConverter::formatDoubletoString(v.z,3) + " " +
					StringConverter::formatDoubletoString(v.w,3);
					osg::ref_ptr<TemplateRecord> record2 = new TemplateRecord(header2);
				record2->setFieldValue("NAME", name);
				record2->setFieldValue("DESCRIPTION", desc);
				record2->setFieldValue("DATA", dataStream);
				database2->insert("OC_DATA", record2);
			}
		}
	}
}

void openSqlite()
{

}