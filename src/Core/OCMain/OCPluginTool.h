#ifndef _OC_PluginTool_H__
#define _OC_PluginTool_H__

#include "FunctionalArchive.h"
#include "Singleton.h"

namespace OC
{
	class _MainExport OCPluginTool : public CSingleton<OCPluginTool>
	{
	public:
		OCPluginTool();
		~OCPluginTool();
		FunctionalArchive* getFunctional();
	private:
		osg::ref_ptr<FunctionalArchive> mFunctionalArchive;
	};

}

#endif // !_OC_PluginTool_H__


