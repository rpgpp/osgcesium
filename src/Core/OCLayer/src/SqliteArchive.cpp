#include "SqliteArchive.h"
#include "FeatureNode.h"

namespace OC
{
	SqliteArchive::SqliteArchive(void)
	{
	}

	SqliteArchive::~SqliteArchive(void)
	{
		
	}

	bool SqliteArchive::open(String filename,ArchiveStatus status,const osgDB::ReaderWriter::Options* options)
	{
		String localFilename = filename;

		String simFilename,ext,path;
		StringUtil::splitFullFilename(localFilename,simFilename,ext,path);
		
		mDirectory = path;

		mMasterFileName = simFilename;

		bool result = false;

		//mDatabase = new SqliteDatabase;

		if (status == CREATE)
		{
			result = mDatabase->create(localFilename);
		}
		else
		{
			if (FileUtil::FileExist(localFilename))
			{
				if (mDatabase->open(localFilename))
				{
					mDatabase->query(mArchiveMeta);
					setSouceType(mArchiveMeta->getDataFrom());
					result = true;
				}
			}
		}

		if (!result)
		{
			mDatabase = NULL;
		}

		if(getSourceType() == DATA_FROM_RAW_DATABASE)
		{
			String connectStr = mArchiveMeta->getConnectString();
			mSpatialArchive = static_cast<IArchive*>(osgDB::openArchive(connectStr,IArchive::READ));
		}

		return result;
	}

	osgDB::ReaderWriter::ReadResult SqliteArchive::readImage(const String& fileName,const osgDB::ReaderWriter::Options* options) const
	{		
		if (!mDatabase.valid() || fileName.empty())
		{
			return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;
		}

		String imageName = fileName;

		osg::ref_ptr<osgDB::Options> localOptions = static_cast<osgDB::Options*>(options == NULL ? new osgDB::Options : options->clone(osg::CopyOp::SHALLOW_COPY));

		localOptions->removePluginStringData("ReadImageFromArchive");

		if (getMeta()->getVersion() < 2019)
		{
			StringStream optionstream;
			optionstream << "dds_flip" << std::endl;
			localOptions->setOptionString(optionstream.str());
		}

		osg::ref_ptr<osg::Image> image/* = mBatchImages.find(imageName)*/;
		
		if(!image.valid())
		{
			if (IArchive::isProtocol(imageName))
			{
				image = osgDB::readImageFile(imageName);
			}
			else
			{
				if (getSourceType() == DATA_FROM_DB)
				{
					if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(osgDB::getLowerCaseFileExtension(imageName)))
					{
						StringStream sstream;
						if (mDatabase->getTextureStream(imageName,sstream))
						{
							image = rw->readImage(sstream,localOptions).takeImage();
						}
					}
				}
				else
				{
					String simpleFileName = osgDB::getSimpleFileName(imageName);
					String file = mDirectory + simpleFileName;
					if (!FileUtil::FileExist(file) && getSourceType() != DATA_FROM_NET)
					{
						file = mDirectory + "images/" + simpleFileName;
					}
					image = osgDB::readImageFile(file,localOptions);
				}

				if (image.valid())
				{
					image->setName(imageName);
					image->setFileName(imageName);
					//mBatchImages.insert(image);
				}
			}
		}

		return image.valid() ? osgDB::ReaderWriter::ReadResult(image.release()) : osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED;
	}

	osgDB::ReaderWriter::WriteResult SqliteArchive::writeImage(const osg::Image& image, const String& fileName,const osgDB::ReaderWriter::Options* ) const
	{
		osgDB::ReaderWriter::WriteResult resutl(osgDB::ReaderWriter::WriteResult::FILE_NOT_HANDLED);

		String ext = osgDB::getLowerCaseFileExtension(fileName);

		osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(ext);

		if (rw != NULL && mDatabase.valid())
		{
			MemoryDataStreamPtr data;
			{
				StringStream sstream;
				if (rw->writeImage(image, sstream).success())
				{
					data.bind(new MemoryDataStream(&sstream));
				}
			}
			if (!data.isNull())
			{
				beginTransaction();
				mDatabase->insertTexture(fileName, data);
				commitTransaction();
				resutl = osgDB::ReaderWriter::WriteResult::FILE_SAVED;
			}
		}

		return resutl;
	}

	bool SqliteArchive::writeFeature(FeatureRecord* record)
	{
		bool result = false;

		String url = record->getFeatureUrl();

		if (url.empty())
		{
			return result;
		}

		beginTransaction();

		bool insert_entity = true;

		if(insert_entity)
		{
			long mid = mShareModelRecord.find(url);

			if (mid < 0)
			{
				mid = mDatabase->insert(record,IDatabase::OP_MODEL);
				mShareModelRecord.insert(url,mid);
			}

			record->setMID(mid);

			long entityID = mDatabase->insert(record,IDatabase::OP_ENTITY);

			result = entityID > -1;

			if (result)
			{
				record->setID(entityID);
			}
		}

		commitTransaction();

		if (result)
		{
			record->onInsert();
		}

		return result;
	}

	bool SqliteArchive::checkUpdate()
	{
		return false;
	}

	void SqliteArchive::recalculate_index()
	{
		
	}

}

