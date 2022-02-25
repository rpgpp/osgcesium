#include "invokeCommand.h"

namespace OC
{
	namespace Server
	{
		void setpublishdir(OC::Json::Value& root)
		{
			OC::String path = OC::JsonPackage::getString(root, "path");
			if (!path.empty())
			{
				path = osgDB::convertFileNameToUnixStyle(path);
				if (path[path.length() - 1] != '/')
				{
					path += "/";
				}
				osgDB::makeDirectory(path);
				Singleton(OC::ConfigManager).setConfig(ConfigNameDataDir, path);
				std::cout << "change publish path:" << path << std::endl;
			}
		}

		void setcachedir(OC::Json::Value& root)
		{
			OC::String path = OC::JsonPackage::getString(root, "path");
			if (!path.empty())
			{
				path = osgDB::convertFileNameToUnixStyle(path);
				if (path[path.length() - 1] != '/')
				{
					path += "/";
				}
				osgDB::makeDirectory(path);
				Singleton(OC::ConfigManager).setConfig(ConfigNameCacheDir, path);
				std::cout << "change cache path:" << path << std::endl;
			}
		}


		String invoke(OC::String jsonStr)
		{
			String result = StringUtil::BLANK;
			try
			{
				Json::Reader reader;
				Json::Value root;
				if (reader.parse(jsonStr, root))
				{
					String method = JsonPackage::getString(root, "method");
					StringUtil::toLowerCase(method);
					if (method == "setpublishdir")
					{
						setpublishdir(root);
					}
					else if (method == "getpublishdir")
					{
						return _getServerDir(ConfigNameDataDir);
					}
					else if (method == "setcachedir")
					{
						setcachedir(root);
					}
					else if (method == "getcachedir")
					{
						result = _getServerDir(ConfigNameCacheDir);
					}
				}
			}
			catch (...)
			{
				std::cout << "invalid invoke in jni." << std::endl;
			}

			return result;
		}
	}
}

