#include "LayerManager.h"

namespace OC
{

#if SingletonConfig == SingletonNormal
	template<>LayerManager* CSingleton<LayerManager>::msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	template<>std::map<int,LayerManager*> CSingleton<LayerManager>::msPidton;
#endif

	LayerManager::LayerManager(void)
	{

	}

	LayerManager::~LayerManager(void)
	{
		
	}
}
