#ifndef _OC_IFC_ARCHIVE_CLASS_H__
#define _OC_IFC_ARCHIVE_CLASS_H__

#include "OCLayer/IArchive.h"

using namespace OC;

class IFCArchive :
	public IArchive
{
public:
	IFCArchive(void);
	~IFCArchive(void);
	virtual const char* libraryName() const { return "OCPlugins IFC"; }
	virtual const char* className() const { return "IFCArchive"; }
	virtual osgDB::ReaderWriter::ReadResult readNode(std::istream& /*fin*/,const Options* =NULL) const;
	virtual osgDB::ReaderWriter::ReadResult readNode(const String& /*fileName*/, const osgDB::ReaderWriter::Options* =NULL) const;
};

#endif
