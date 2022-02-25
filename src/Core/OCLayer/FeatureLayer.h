#ifndef _OC_FEATURELAYER_H__
#define _OC_FEATURELAYER_H__

#include "ILayer.h"

namespace OC
{
	class _LayerExport CFeatureLayer : public ILayer
	{
	public:
		CFeatureLayer(String layerID = StringUtil::BLANK);
		~CFeatureLayer(void);
	};
}
#endif
