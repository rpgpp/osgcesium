#include "OCResource.h"
#include "OCMain/Environment.h"

namespace OC
{
#if SingletonConfig == SingletonNormal
	template<>OCResource* CSingleton<OCResource>::msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	template<>std::map<int,OCResource*> CSingleton<OCResource>::msPidton;
#endif

	OCResource::OCResource(void)
	{
		String filename = Singleton(Environment).getAppDir() + "data/resource.db";
		String conectStr = "driver:sqlite;location:" + filename;
		mArchive = static_cast<IArchive*>(osgDB::Registry::instance()->openArchive(conectStr,osgDB::ReaderWriter::READ,0,NULL).takeArchive());
	}

	OCResource::~OCResource(void)
	{
	}

	IArchive* OCResource::getArchive()
	{
		return mArchive.get();
	}

	osg::Image* OCResource::readImage(String file)
	{
		if(mArchive.valid())
		{
			return mArchive->readImage(file).takeImage();
		}

		return NULL;
	}

	inline bool validFile(String filename)
	{
		return FileUtil::FileExist(filename) && FileUtil::FileSize(filename) > 0;
	}

	osg::Node* OCResource::readNode(String file)
	{
		if(mArchive.valid())
		{
			return mArchive->readNode(file).takeNode();
		}

		return NULL;
	}
}

