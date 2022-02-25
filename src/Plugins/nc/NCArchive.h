#ifndef _OC_NC_ARCHIVE_CLASS_H__
#define _OC_NC_ARCHIVE_CLASS_H__

#include "OCLayer/IArchive.h"

using namespace OC;

class NcArchive : public IArchive
{
public:
	NcArchive(void);
	~NcArchive(void);
	virtual const char* libraryName() const { return "OCPlugins NC"; }
	virtual const char* className() const { return "NcArchive"; }
	virtual bool acceptsExtension(const String& extension) const { return osgDB::ReaderWriter::acceptsExtension(extension); }
	virtual osgDB::ReaderWriter::ReadResult readNode(std::istream& /*fin*/, const Options* = NULL) const;
	virtual osgDB::ReaderWriter::ReadResult readNode(const String& /*fileName*/, const osgDB::ReaderWriter::Options* = NULL) const;
	virtual osgDB::ReaderWriter::ReadResult readImage(const String& /*fileName*/, const osgDB::ReaderWriter::Options* = NULL) const;
	virtual osgDB::ReaderWriter::ReadResult readObject(const String& /*fileName*/, const osgDB::ReaderWriter::Options* = NULL) const;
	virtual osgDB::ReaderWriter::WriteResult writeImage(const osg::Image& /*image*/, const String& /*fileName*/, const osgDB::ReaderWriter::Options* = NULL) const;
	virtual osgDB::ReaderWriter::WriteResult writeImage(const osg::Image& /*image*/, std::ostream& /*fout*/, const osgDB::ReaderWriter::Options* = NULL) const;
};

#endif
