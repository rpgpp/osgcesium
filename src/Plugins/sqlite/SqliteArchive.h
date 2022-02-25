#pragma  once

#include "OCLayer/IArchive.h"

namespace OC
{
	class SqliteArchive : public IArchive
	{
	public:
		SqliteArchive();
		~SqliteArchive();
		virtual const char* libraryName() const { return "OCPlugins Sqlite"; }
		virtual const char* className() const { return "OC::SqliteArchive"; }
		virtual bool open(String filename,ArchiveStatus status,const osgDB::ReaderWriter::Options* options);
		virtual osgDB::ReaderWriter::ReadResult openArchive(const String& /*fileName*/,ArchiveStatus, unsigned int =4096, const osgDB::Options* =NULL) const;
		virtual osgDB::ReaderWriter::ReadResult readNode(const String& /*fileName*/, const osgDB::ReaderWriter::Options* = NULL) const;
	};
}