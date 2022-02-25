#include "FeatureLayer.h"
#include "LayerManager.h"

namespace OC
{
	CFeatureLayer::CFeatureLayer(String layerID)
		:ILayer(SingletonPtr(LayerManager),layerID)
	{
		
	}

	CFeatureLayer::~CFeatureLayer(void)
	{
		
	}
}

