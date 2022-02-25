#include "VolumeManager.h"
#include "OCVolume/Volume.h"

namespace OC
{
	using namespace Volume;
    namespace Server
    {
		String VolumeManager::getVolumeFile(const String name, const String varname, const String ext, const String dir, const String version)
		{
			String dataDir = _getServerDir(ConfigNameDataDir);
			String cacheDir = _getServerDir(ConfigNameCacheDir);

			String file_var_dotw3dm = cacheDir + name + "." + varname + ".w3dm";

			String error = StringUtil::BLANK;
			if (!FileUtil::FileExist(file_var_dotw3dm))
			{
				String filename = dataDir + name + "." + ext;
				osg::ref_ptr<CVolumeObject> vo = CVolume::convert(filename, varname);
				if (vo.valid())
				{
					vo->writeToW3DM(file_var_dotw3dm);
				}
				else
				{
					error = " error";
				}
			}

			Singleton(LogManager).stream() << file_var_dotw3dm << error;

			return file_var_dotw3dm;
		}
    }
}