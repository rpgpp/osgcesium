#ifndef _SQLITE_ARCHIVE_H_
#define _SQLITE_ARCHIVE_H_

#include "IArchive.h"

namespace OC
{
	class _LayerExport SqliteArchive : public IArchive
	{
	public:
		SqliteArchive(void);
		virtual ~SqliteArchive(void);
		virtual osgDB::ReaderWriter::ReadResult readImage(const String& fileName,const osgDB::ReaderWriter::Options* =NULL) const;
		virtual osgDB::ReaderWriter::WriteResult writeImage(const osg::Image& image, const String& fileName,const osgDB::ReaderWriter::Options* =NULL) const;
	public:
		bool open(String filename,ArchiveStatus status,const osgDB::ReaderWriter::Options* options=NULL);
		virtual bool writeFeature(FeatureRecord* record);
		void recalculate_index();
		virtual bool checkUpdate();
	private:
		mutable ObserverObjectCacher<osg::Image>				mBatchImages;

		//for write feature
		StringLongMap											mShareTileRecord;
		StringLongMap											mShareModelRecord;
	};
}



#endif
