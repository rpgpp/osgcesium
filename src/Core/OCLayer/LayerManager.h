#ifndef __LAYER_MANAGER__H_
#define __LAYER_MANAGER__H_

#include "ILayer.h"
#include "OCMain/Singleton.h"

namespace OC
{
	class _LayerExport LayerManager : public CSingleton<LayerManager>
	{
	public:
		LayerManager(void);
		~LayerManager(void);
	};
}

#endif
