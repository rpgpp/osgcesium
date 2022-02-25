#include "IFCArchive.h"
#include "IFCConverter.h"

IFCArchive::IFCArchive(void)
{
	supportsExtension("ifc","OC ifc data format");
}


IFCArchive::~IFCArchive(void)
{
}

osgDB::ReaderWriter::ReadResult IFCArchive::readNode(std::istream& /*fin*/,const Options*) const
{
	return nullptr;
}

osgDB::ReaderWriter::ReadResult IFCArchive::readNode(const String& fileName, const osgDB::ReaderWriter::Options*) const
{
	IFCConverter converter;
	return converter.read(fileName.c_str());
}

REGISTER_OSGPLUGIN(ifc, IFCArchive)
