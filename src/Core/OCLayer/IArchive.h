#ifndef _IARCHIVE_H__
#define _IARCHIVE_H__

#include "IDatabase.h"
#include "OCMain/ObjectContainer.h"

namespace OC
{
	class _LayerExport IArchive : public osgDB::Archive
	{
	public:
		IArchive();
		IArchive(const IArchive& eh,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY){}
		virtual ~IArchive();

		META_Object(OC,IArchive);

		String getArchiveFileName() const;
		void setArchiveFileName(String name){mArchiveFileName = name;}

		String getMasterFileName() const;
		void setMasterFileName(String name){mMasterFileName = name;}

		virtual bool acceptsExtension(const String& extension) const { return osgDB::ReaderWriter::acceptsExtension(extension); }
		virtual void close(){}
		virtual bool fileExists(const String& filename) const{return false;}
		virtual bool getFileNames(osgDB::Archive::FileNameList& fileNameList) const{return false;}
		virtual osgDB::FileType getFileType(const String& filename) const{return osgDB::FILE_NOT_FOUND;}

		virtual osgDB::ReaderWriter::ReadResult readObject(const String& /*fileName*/, const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED;}
		virtual osgDB::ReaderWriter::ReadResult readImage(const String& /*fileName*/,const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED;}
		virtual osgDB::ReaderWriter::ReadResult readHeightField(const String& /*fileName*/,const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED;}
		virtual osgDB::ReaderWriter::ReadResult readNode(const String& /*fileName*/, const osgDB::ReaderWriter::Options* =NULL) const;
		virtual osgDB::ReaderWriter::ReadResult readShader(const String& /*fileName*/, const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED;}

		virtual osgDB::ReaderWriter::WriteResult writeObject(const osg::Object& /*obj*/, const String& /*fileName*/,const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED;}
		virtual osgDB::ReaderWriter::WriteResult writeImage(const osg::Image& /*image*/, const String& /*fileName*/,const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED;}
		virtual osgDB::ReaderWriter::WriteResult writeHeightField(const osg::HeightField& /*heightField*/, const String& /*fileName*/,const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED;}
		virtual osgDB::ReaderWriter::WriteResult writeNode(const osg::Node& /*node*/, const String& /*fileName*/,const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED;}
		virtual osgDB::ReaderWriter::WriteResult writeShader(const osg::Shader& /*shader*/, const String& /*fileName*/,const osgDB::ReaderWriter::Options* =NULL) const{return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED;}
	public:
		virtual void extractModel(String directory){}
		virtual void extractTexture(String directory){}
		virtual bool open(String filename,ArchiveStatus status,const osgDB::ReaderWriter::Options* options=NULL){return false;}
		virtual IDatabase* getDatabase() const;

		void setDirectory(const String dir);
		String getDirectory() const;

		void beginTransaction() const;
		void commitTransaction() const;

		int getCommitNum();
		void setCommitNum(int num);
	protected:
		String													mDirectory;
		String													mMasterFileName;
		String													mArchiveFileName;
		mutable int												mCommitNum;
		mutable int												mCommitCount;
		osg::ref_ptr<IDatabase>									mDatabase;
	};
}

#endif