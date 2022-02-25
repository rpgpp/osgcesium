#ifndef _OC_SERVER__DEFINE_H_
#define _OC_SERVER__DEFINE_H_

#if defined(_MSC_VER)
#ifndef OCServer_EXPORTS
#define _OCServerExport  __declspec(dllimport)
#else
#define _OCServerExport  __declspec(dllexport)
#endif
#else
#define _OCServerExport
#endif

#include "OCMain/Environment.h"
#include "OCMain/ConfigManager.h"

namespace OC
{
	namespace Server
	{
		class COCServer;
		class OCRegister;

#define ConfigNameDataDir "data_dir"
#define ConfigNameCacheDir "cache_dir"
#define _getServerDir(name) Singleton(ConfigManager).getStringValue(name)

	}
}

#endif