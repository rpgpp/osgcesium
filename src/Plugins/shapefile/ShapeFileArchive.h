#pragma  once

#include "OCLayer/IArchive.h"

namespace OC
{
	namespace Modeling
	{
		class ShapeFileArchive:public IArchive
		{
		public:
			ShapeFileArchive();
			~ShapeFileArchive();
			virtual const char* libraryName() const { return "OCPlugins ShapeFile"; }
			virtual const char* className() const { return "Shapefile"; }
			virtual bool acceptsExtension(const String& extension) const { return osgDB::ReaderWriter::acceptsExtension(extension); }
			virtual bool open(String filename,ArchiveStatus status,const osgDB::ReaderWriter::Options* options);
			virtual osgDB::ReaderWriter::ReadResult openArchive(const String& /*fileName*/,ArchiveStatus, unsigned int =4096, const osgDB::Options* =NULL) const;
			virtual bool writeFeature(FeatureRecord* record);
		};
	}
}