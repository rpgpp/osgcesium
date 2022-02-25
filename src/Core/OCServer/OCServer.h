#ifndef _OC_SERVER_CLASS_H__
#define _OC_SERVER_CLASS_H__

#include "OCServerDefine.h"
#include "OCMain/Singleton.h"
#include "OCMain/DataStream.h"
#include "OCLayer/OCResource.h"

namespace OC
{
	namespace Server
	{
		class _OCServerExport COCServer : public CSingleton<COCServer>
		{
		public:
			COCServer();
			~COCServer();
			void init();
			String invoke(String jsonStr);
			String getDataInfo(String method, String param, String filter);
			MemoryDataStreamPtr getByteData(String method, String param, String filter);
			MemoryDataStreamPtr getByteData(String filename);
		private:
			OCResource*								mOCResource;
			Environment*							mEnvironment;
            ConfigManager*							mConfigManager;
			std::map<String, MemoryDataStreamPtr>	mMemoryCaches;
        };
	}
}



#endif
