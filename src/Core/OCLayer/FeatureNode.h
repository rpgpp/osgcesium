#ifndef _FEATURE_NODE_CLASS_H__
#define _FEATURE_NODE_CLASS_H__

#include "SceneNode.h"
#include "FeatureRecord.h"

namespace OC
{
	class _LayerExport CFeatureNode : public CSceneNode
	{
	public:
		CFeatureNode(CFeature* owner);
		virtual	~CFeatureNode(void);
	};
}

#endif
