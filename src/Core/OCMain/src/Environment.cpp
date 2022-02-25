#include "Environment.h"
#include "LogManager.h"
#include "OCPluginTool.h"

namespace OC
{
#if SingletonConfig == SingletonNormal
	Environment* msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	std::map<int,Environment*> msPidton;
#endif

	Environment::Environment(uint32 magic)
		:mLogManager(NULL)
	{
		std::locale::global(std::locale("C"));
		setlocale(LC_ALL, "chs");
#if SingletonConfig == SingletonNormal
		msSingleton = this;
#elif SingletonConfig == SingletonProcess
		msPidton[CORE_CURRENT_PID] = this;
#endif
		mLogManager = new LogManager;
		mOCPluginTool = new OCPluginTool;
		if(magic != 0x6C6777)
		{
			osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("license");
			osg::ref_ptr<osgDB::Archive> archive = rw->openArchive("OC", osgDB::ReaderWriter::READ).takeArchive();
			if (!archive.valid())
			{
				delete this;
				delete mLogManager;
				delete mLogManager;
			}
		}
	}

	Environment::~Environment(void)
	{
		CORE_SAFE_DELETE(mLogManager);
		CORE_SAFE_DELETE(mOCPluginTool);
	}

	Environment* Environment::getSingletonPtr()
	{
#if SingletonConfig == SingletonNormal
		return msSingleton;
#elif SingletonConfig == SingletonProcess
		int pid = CORE_CURRENT_PID;
		if (msPidton.find(pid) == msPidton.end())
		{
			return new Environment;
		}
		return msPidton[pid];
#endif
	}
	 
	Environment& Environment::getSingleton()
	{
#if SingletonConfig == SingletonNormal
		return *msSingleton;
#elif SingletonConfig == SingletonProcess
		int pid = CORE_CURRENT_PID;
		if (msPidton.find(pid) == msPidton.end())
		{
			return *(new Environment);
		}
		return *msPidton[pid];
#endif

	}

	String Environment::getAppDir()
	{
		return mApplicationDir;
	}

	String Environment::getTempDir()
	{
		String path = getAppDir() + "temp";
		bool exist = FileUtil::makeDirectory(path);
		if(exist)
		{
			return path + "/";
		}
		return getAppDir();
	}

	String Environment::getLogPath()
	{
		return mApplicationDir + "Core.log";
	}

	void Environment::setAppDir(String dir)
	{
		replace(dir.begin(),dir.end(),'\\','/');
		mApplicationDir = dir;

		if (!mApplicationDir.empty())
		{
			if (*mApplicationDir.rbegin() != '/')
			{
				mApplicationDir.push_back('/');
			}
		}
	}
}