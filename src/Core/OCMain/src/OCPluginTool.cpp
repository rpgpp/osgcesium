#include "OCPluginTool.h"

namespace OC
{
#if SingletonConfig == SingletonNormal
	template<>OCPluginTool* CSingleton<OCPluginTool>::msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	template<>std::map<int, OCPluginTool*> CSingleton<OCPluginTool>::msPidton;
#endif

	OCPluginTool::OCPluginTool()
	{
		mFunctionalArchive = static_cast<FunctionalArchive*>(osgDB::Registry::instance()->openArchive(".func", osgDB::ReaderWriter::READ, 0, NULL).takeArchive());
	}

	OCPluginTool::~OCPluginTool()
	{

	}

	FunctionalArchive* OCPluginTool::getFunctional()
	{
		return mFunctionalArchive.get();
	}
}