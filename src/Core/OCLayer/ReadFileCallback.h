#ifndef _OC_READFILECALLBACK_H__
#define _OC_READFILECALLBACK_H__

#include "IArchive.h"

namespace OC
{
	class _LayerExport OCReadFileCallback  : public osgDB::ReadFileCallback
	{
	public:
		OCReadFileCallback();
		~OCReadFileCallback();

		virtual osgDB::ReaderWriter::ReadResult openArchive(const String& fileName, osgDB::ReaderWriter::ArchiveStatus status, unsigned int indexBlockSizeHint, const osgDB::Options* options);
		virtual osgDB::ReaderWriter::ReadResult readObject(const String& fileName,const osgDB::Options* options);
		virtual osgDB::ReaderWriter::ReadResult readImage(const String& fileName,const osgDB::Options* options);
		virtual osgDB::ReaderWriter::ReadResult readHeightField(const String& fileName,const osgDB::Options* options);
		virtual osgDB::ReaderWriter::ReadResult readShader(const String& fileName,const osgDB::Options* options);
		virtual osgDB::ReaderWriter::ReadResult readNode(const String& filename, const osgDB::Options* options);

	private:
		osg::Node* getErrorNode();
		inline bool is_error(String url);
		inline void mark_error(String url);
		StringIntMap							mErrorRequests;
		osg::ref_ptr<osg::Node>					mErrorNode;
		ObserverObjectCacher<IArchive>			mCacheArchives;
	};
}




#endif
