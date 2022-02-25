#include "ReadFileCallback.h"
#include "OCResource.h"
#include "OCMain/JsonHelper.h"

namespace OC
{
	OCReadFileCallback::OCReadFileCallback()
	{
		
	}

	OCReadFileCallback::~OCReadFileCallback(void)
	{
	}

	osg::Node* OCReadFileCallback::getErrorNode()
	{
		if (!mErrorNode.valid())
		{
			osg::Geode* geode = new osg::Geode;
			geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),0.5)));
			mErrorNode = geode;
		}
		return mErrorNode;
	}

	bool OCReadFileCallback::is_error(String url)
	{
		return mErrorRequests.find(url) > -1;
	}

	void OCReadFileCallback::mark_error(String url)
	{
		mErrorRequests.insert(url,mErrorRequests.find(url) + 1);
	}

	osgDB::ReaderWriter::ReadResult OCReadFileCallback::openArchive(const String& fileName, osgDB::ReaderWriter::ArchiveStatus status, unsigned int indexBlockSizeHint, const osgDB::Options* options)
	{
		osgDB::ReaderWriter::ReadResult rresult = osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;

		String url = fileName;

		osg::ref_ptr<IArchive> archive = mCacheArchives.find(url);

		if (archive.valid())
		{
			return archive.release();
		}

		CORE_LOCK_RW_MUTEX_WRITE(mCacheArchives.BatchLock)

		if (StringUtil::startsWith(url,"http://"))
		{
			return rresult;
		}
		else if (StringUtil::startsWith(url,"server://"))
		{
			
		}
		else if (StringUtil::startsWith(url,"driver:"))
		{
			StringMap sm = JsonHelper::parseStyleString(url);
			String ext = sm["driver"];
			if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(ext))
			{
				osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
				options->setOptionString(url);
				archive = dynamic_cast<IArchive*>(rw->openArchive("." + ext,status,0,options).takeArchive());
				if (archive.valid())
				{
					archive->setName(url);
					mCacheArchives.insert(archive);
				}
			}

			return osgDB::ReaderWriter::ReadResult(archive.release());
		}
		else
		{
			
		}

		return osgDB::Registry::instance()->openArchiveImplementation(fileName, status, indexBlockSizeHint, options);
	}

	osgDB::ReaderWriter::ReadResult OCReadFileCallback::readObject(const String& fileName,const osgDB::Options* options)
	{
		return osgDB::Registry::instance()->readObjectImplementation(fileName,options);
	}

	osgDB::ReaderWriter::ReadResult OCReadFileCallback::readImage(const String& fileName,const osgDB::Options* options)
	{
		String url = fileName;

		if (is_error(url))
		{
			return NULL;
		}

		if (options)
		{
			String archive_name = options->getPluginStringData("ReadImageFromArchive");
			if (!archive_name.empty())
			{
				osg::ref_ptr<IArchive> archie = mCacheArchives.find(archive_name);
				if (archie.valid())
				{
					return archie->readImage(url,options);
				}
			}
		}

		osgDB::ReaderWriter::ReadResult result;
		
		if (StringUtil::startsWith(url,"global://"))
		{
			String file = url.substr(9,url.length() - 9);
			osg::ref_ptr<osg::Image> image = Singleton(OCResource).readImage(file);
			if (image.valid())
			{
				image->setFileName(url);
				return image.release();
			}
		}
		else if (StringUtil::startsWith(url,"server://"))
		{
			
		}
		else if (StringUtil::startsWith(url,"http://"))
		{

		}
		else if (StringUtil::startsWith(url,"label://"))
		{
			
		}

		return osgDB::Registry::instance()->readImageImplementation(url,options);
	}

	osgDB::ReaderWriter::ReadResult OCReadFileCallback::readHeightField(const String& fileName,const osgDB::Options* options)
	{
		return osgDB::Registry::instance()->readHeightFieldImplementation(fileName,options);
	}

	osgDB::ReaderWriter::ReadResult OCReadFileCallback::readShader(const String& fileName,const osgDB::Options* options)
	{
		osgDB::ReaderWriter::ReadResult rresult = osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;

		if (StringUtil::startsWith(fileName, "shaders/"))
		{
			String file = Singleton(Environment).getAppDir() + "Resources/" + fileName;
			rresult = osgDB::Registry::instance()->readShaderImplementation(file, options);
		}

		if (!rresult.validShader())
		{
			rresult = osgDB::Registry::instance()->readShaderImplementation(fileName,options);
		}

		return rresult;
	}

	osgDB::ReaderWriter::ReadResult OCReadFileCallback::readNode(const String& filename, const osgDB::Options* options)
	{
		osgDB::ReaderWriter::ReadResult rresult = osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;

		String url = filename;

		if (is_error(url) && "osgearth_engine_mp_tile" != osgDB::getLowerCaseFileExtension(filename))
		{
			return getErrorNode();
		}

		if (options)
		{
			String archive_name = options->getPluginStringData("ReadNodeFromArchive");
			if (!archive_name.empty())
			{
				osg::ref_ptr<IArchive> archie = mCacheArchives.find(archive_name);
				if (archie.valid())
				{
					return archie->readNode(url,options);
				}
			}
		}

		if (StringUtil::startsWith(url,"global://"))
		{
			String file = url.substr(9,url.length() - 9);
			osg::ref_ptr<osg::Node> node = Singleton(OCResource).readNode(file);
			rresult = node.release();
		}

		if (!rresult.validNode())
		{
			rresult = osgDB::Registry::instance()->readNodeImplementation(url,options);
		}

		if (!rresult.validNode())
		{
			mark_error(url);
		}

		return rresult;
	}
}
