#ifndef _OC_ModelArchive_H__
#define _OC_ModelArchive_H__

#include "ModelDefine.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport ModelArchive : public osgDB::Archive
		{
		public:
			ModelArchive();
			virtual ~ModelArchive();
			virtual IModelTable* getModelTable(ModelDataManager* owner);
		protected:
			osgDB::ReaderWriter::ReadResult readObject(const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::ReadResult readImage(const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::ReadResult readHeightField(const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::ReadResult readNode(const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::ReadResult readShader(const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::ReadResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::WriteResult writeObject(const osg::Object&, const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::WriteResult writeImage(const osg::Image&, const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::WriteResult writeHeightField(const osg::HeightField&, const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::WriteResult writeNode(const osg::Node&, const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED; }
			osgDB::ReaderWriter::WriteResult writeShader(const osg::Shader&, const String&, const osgDB::ReaderWriter::Options* = NULL) const { return osgDB::ReaderWriter::WriteResult::NOT_IMPLEMENTED; }
			String getArchiveFileName() const { return StringUtil::BLANK; }
			String getMasterFileName() const { return StringUtil::BLANK; }
			void close() {}
			bool fileExists(const String& filename) const { return false; }
			bool getFileNames(osgDB::Archive::FileNameList& fileNameList) const { return false; }
			osgDB::FileType getFileType(const String& filename) const { return osgDB::FILE_NOT_FOUND; }
			bool acceptsExtension(const String& extension) const { return osgDB::ReaderWriter::acceptsExtension(extension); }
		};
	}
}

#endif // ! _OC_FunctionalArchive_H__


