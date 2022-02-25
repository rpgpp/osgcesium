#ifndef _OC_FunctionalArchive_H__
#define _OC_FunctionalArchive_H__

#include "BaseDefine.h"

namespace OC
{
	class _MainExport FunctionalArchive : public osgDB::Archive
	{
	public:
		FunctionalArchive();
		virtual ~FunctionalArchive();

		virtual CVector2 gauss2LH(CVector2 coord);
		virtual String invoke(StringVector params) {
			return StringUtil::BLANK;
		}

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

#endif // ! _OC_FunctionalArchive_H__


