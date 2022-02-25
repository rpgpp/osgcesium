#include "PipeArchive.h"
#include "PipeTable.h"

PipeArchive::PipeArchive(void)
{
	supportsExtension("pipe", "OC pipe modeling");
}

PipeArchive::~PipeArchive(void)
{

}

IModelTable* PipeArchive::getModelTable(ModelDataManager* owner)
{
	return new PipeTable(owner);
}

osgDB::ReaderWriter::ReadResult PipeArchive::openArchive(const String& fileName, osgDB::ReaderWriter::ArchiveStatus status, unsigned int dep, const osgDB::Options* options) const
{
	String ext = osgDB::getLowerCaseFileExtension(fileName);
	if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;
	return new PipeArchive;
}

REGISTER_OSGPLUGIN(func, PipeArchive)
