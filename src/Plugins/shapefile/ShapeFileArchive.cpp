#include "ShapeFileArchive.h"
#include "ShapeFileDatabase.h"
#include "OCMain/JsonHelper.h"

using namespace OC;
using namespace OC::Modeling;
 
ShapeFileArchive::ShapeFileArchive()
{
	supportsExtension("ShapeFile","ShapeFile modeling data format");
	supportsExtension("shp","ShapeFile modeling data format");
}

ShapeFileArchive::~ShapeFileArchive()
{
}

bool ShapeFileArchive::open(String filename,ArchiveStatus status,const osgDB::ReaderWriter::Options* options)
{
	osg::ref_ptr<ShapeFileDatabase> md = new ShapeFileDatabase;

	String dest = filename;

	StringMap sm = JsonHelper::parseStyleString(filename);
	String optDest = sm["ip"];
	if (!optDest.empty())
	{
		//String optCatalog = sm["catalog"];
		//if (!StringUtil::endsWith(optDest,".shp"))
		//{
		//	optDest += "/" + optCatalog;
		//}
		dest = optDest;
	}

	if(status == CREATE)
	{
		if (md->create(dest))
		{
			mDatabase = md;
		}
	}
	else
	{
		if (md->connect(dest))
		{
			mDatabase = md;
		}
	}

	return mDatabase.valid();
}

osgDB::ReaderWriter::ReadResult ShapeFileArchive::openArchive(const String& fileName,ReaderWriter::ArchiveStatus status, unsigned int dep, const Options* options) const
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

	osg::ref_ptr<ShapeFileArchive> archive = new ShapeFileArchive;
	if (archive->open(connectStr,status,options))
	{
		return archive.get();
	}

	return ReadResult::FILE_NOT_FOUND; 
}

bool ShapeFileArchive::writeFeature(FeatureRecord* record)
{
	return static_cast<ShapeFileDatabase*>(mDatabase.get())->writeFeature(record);
}

REGISTER_OSGPLUGIN(ShapeFile, ShapeFileArchive)
