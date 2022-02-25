#include "OCServer.h"
#include "OCLayer/ReadFileCallback.h"
#include "VolumeManager.h"
#include "OCRegister.h"
#include "invokeCommand.h"
#include "OCVolume/Volume.h"

namespace OC
{
#if SingletonConfig == SingletonNormal
	template<>Server::COCServer* CSingleton<Server::COCServer>::msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	template<>std::map<int,Server::COCServer*> CSingleton<Server::COCServer>::msPidton;
#endif
	using namespace Volume;

	namespace Server{

	inline String getCurrentPath()
	{
#if _DEBUG
		HANDLE hDllhandle = GetModuleHandleA("OCServerd.dll");
#else
		HANDLE hDllhandle = GetModuleHandleA("OCServer.dll");
#endif
		if (!hDllhandle)
		{
			return StringUtil::BLANK;
		}
		char buff[MAX_PATH];
		memset(buff,0,MAX_PATH);
		GetModuleFileNameA((HMODULE)hDllhandle,buff,MAX_PATH);
		String fullPath = buff;
		size_t pos = fullPath.find_last_of('\\');
		fullPath = fullPath.substr(0, pos);
		fullPath += "/";
		return StringUtil::replaceAll(fullPath,"\\","/");
	}

	COCServer::COCServer()
		:mEnvironment(NULL)
		,mConfigManager(NULL)
		,mOCResource(NULL)
	{

	}

	COCServer::~COCServer()
	{
		CORE_SAFE_DELETE(mConfigManager);
		CORE_SAFE_DELETE(mEnvironment);
		CORE_SAFE_DELETE(mOCResource);
	}

	void COCServer::init()
	{
		String workspace = getCurrentPath();
		mEnvironment = new Environment(0x6C6777);
		mEnvironment->setAppDir(workspace);
		mConfigManager = new ConfigManager(workspace + "config" + "/config.ini");

		Singleton(LogManager).createLog("",false,true);

		osgDB::Registry::instance()->setReadFileCallback(new OCReadFileCallback);

		String dataPath = mConfigManager->getStringValue(ConfigNameDataDir, Singleton(Environment).getTempDir());
		String cachePath = mConfigManager->getStringValue(ConfigNameCacheDir, Singleton(Environment).getTempDir());
		cachePath = osgDB::convertFileNameToUnixStyle(cachePath);
		dataPath = osgDB::convertFileNameToUnixStyle(dataPath);
		if (cachePath[cachePath.length() - 1] != '/') cachePath += "/";
		if (dataPath[dataPath.length() - 1] != '/') dataPath += "/";

		mConfigManager->setConfig(ConfigNameDataDir, dataPath);
		mConfigManager->setConfig(ConfigNameCacheDir, cachePath);

		mOCResource = new OCResource;
		osgDB::makeDirectory(cachePath);

		std::cout << "* Workspace: " << workspace << std::endl;
		std::cout << "* PublishPath:" << dataPath << std::endl;
		std::cout << "* CachesPath: " << cachePath << std::endl;
	}

	String COCServer::getDataInfo(String method,String param,String filter)
	{
		String result = StringUtil::BLANK;

		StringUtil::toLowerCase(method);
		StringVector params = StringUtil::split(param , ";");

		String publishDir = _getServerDir(ConfigNameDataDir);

		if (method == "nc" && params.size() > 2)
		{
			String ext = "nc";
			String version = "1.0";
			if (params.size() > 3)
			{
				ext = params[3];
			}
			if (params.size() > 4)
			{
				version = params[4];
			}
			result = VolumeManager::getVolumeFile(params[1], params[2], ext, publishDir, version);
		}

		return result;
	}

	MemoryDataStreamPtr COCServer::getByteData(String bodydata)
	{
		MemoryDataStreamPtr dataStream;
		String filename = bodydata;
		StringUtil::toLowerCase(filename);
		String name, ext, path;
		StringUtil::splitFullFilename(filename, name, ext, path);
		String varname = StringUtil::BLANK;
		if (ext == "bz2")
		{
			varname = "DBZ";
			String name = osgDB::getNameLessExtension(osgDB::getSimpleFileName(filename));
			StringUtil::toUpperCase(name);
			StringVector names = StringUtil::split(osgDB::getNameLessAllExtensions(name), "_");
			for (auto str : names)
			{
				if (str == "SA" || str == "SB" || str == "CA" || str == "FMT")
				{
					varname = "EF6A0A74";
					break;
				}
			}
		}

		filename = StringUtil::decodeURI(filename);

		bool cacheInMemory = Singleton(ConfigManager).getBoolValue("OCServerCacheInMemory");
		if (cacheInMemory)
		{
			if (mMemoryCaches.find(filename) != mMemoryCaches.end())
			{
				return mMemoryCaches[filename];
			}
		}

		osg::ref_ptr<CVolumeObject> vo = CVolume::convert(filename, varname);
		if (vo.valid())
		{
			StringStream sstream;
			vo->writeToStream(sstream);
			dataStream = MemoryDataStreamPtr(new MemoryDataStream(&sstream, false, true));
			if (!dataStream.isNull())
			{
				dataStream->name() = name + "." + varname + ".w3dm";
			}

			if (cacheInMemory)
			{
				mMemoryCaches[filename] = dataStream;
			}
		}

		return dataStream;
	}

	MemoryDataStreamPtr COCServer::getByteData(String method, String param, String filter)
	{
		String publishDir = _getServerDir(ConfigNameDataDir);
		MemoryDataStreamPtr dataStream;
		StringUtil::toLowerCase(method);
		StringVector params = StringUtil::split(param, ";");
		if (method == "register" && params.size() > 5)
		{
			StringStream sstream;
			sstream << OCRegister::getRegister(params);
			dataStream = MemoryDataStreamPtr(new MemoryDataStream(&sstream));
			return dataStream;
		}

		return dataStream;
	}

	String COCServer::invoke(String jsonStr)
	{
		return OC::Server::invoke(jsonStr);
	}

}}//namespace
