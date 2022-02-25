#include "IArchive.h"

namespace OC
{
	IArchive::IArchive()
		:mCommitNum(1000)
		,mCommitCount(0)
	{

	}

	IArchive::~IArchive()
	{
		if (mDatabase.valid())
		{
			mDatabase->commitTransaction();
		}
	}
	
	IDatabase* IArchive::getDatabase() const
	{
		return mDatabase.get();
	}

	String IArchive::getArchiveFileName() const
	{
		return mArchiveFileName;
	}

	String IArchive::getMasterFileName() const
	{
		return mMasterFileName;
	}

	void IArchive::setDirectory(const String dir)
	{
		mDirectory = dir;
		if (!StringUtil::endsWith(dir,"/"))
		{
			mDirectory += "/";
		}
	}

	String IArchive::getDirectory() const
	{
		return mDirectory;
	}

	int IArchive::getCommitNum()
	{
		return mCommitNum;
	}

	void IArchive::setCommitNum(int num)
	{
		mCommitNum = num;
	}

	void IArchive::beginTransaction() const
	{
		mCommitCount++;
		if (mDatabase.valid())
		{
			mDatabase->beginTransaction();
		}
	}

	void IArchive::commitTransaction() const
	{
		if (mCommitCount >= mCommitNum && mDatabase.valid())
		{
			mCommitCount = 0;
			mDatabase->commitTransaction();
		}
	}

	osgDB::ReaderWriter::ReadResult IArchive::readNode(const String& fileName, const osgDB::ReaderWriter::Options* options) const
	{
		return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;
	}

}