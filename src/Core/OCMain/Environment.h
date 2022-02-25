#ifndef __APPLICATION_ENVIRONMENT__H_
#define __APPLICATION_ENVIRONMENT__H_

#include "BaseDefine.h"

namespace OC
{
	class _MainExport Environment
	{
	public:
		Environment(uint32 magic = 0);
		~Environment(void);

		static Environment* getSingletonPtr();
		static Environment& getSingleton();

		void   setAppDir(String dir);
		String getAppDir();
		String getLogPath();
		String getTempDir();
	private:
		String					mApplicationDir;
		LogManager*				mLogManager;
		OCPluginTool*			mOCPluginTool;
	};
}


#endif
