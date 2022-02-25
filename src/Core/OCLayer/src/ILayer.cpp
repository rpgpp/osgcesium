#include "ILayer.h"
#include "SceneNode.h"

namespace OC
{
	ILayer::ILayer(LayerManager* creator,String ID):
		mVisible(false),
		mCreator(creator),
		mID(ID),
		mLoaded(false)
	{
		if (mID.empty())
		{
			mID = StringUtil::GenGUID();
		}
		mLayerConfig = new CLayerConfig;
	}

	ILayer::~ILayer(void)
	{
		
	}

	bool ILayer::getVisible()
	{
		return mVisible;
	}

	void ILayer::setVisible(bool visible)
	{
		if (mSceneNode.valid())
		{
			mSceneNode->setVisible(visible);
		}
		mVisible = visible;
	}

	void ILayer::setName(String name)
	{
		mName = name;
	}

	String ILayer::getName()
	{
		return mName;
	}

	void ILayer::setID(String id)
	{
		mID = id;
	}

	String ILayer::getID()
	{
		return mID;
	}

	void ILayer::setURL(String url)
	{
		mURL = url;
	}

	String ILayer::getURL()
	{
		return mURL;
	}

	void ILayer::setConfig(CLayerConfig* config)
	{
		mLayerConfig = config;
	}

	CLayerConfig& ILayer::getConfig()
	{
		return *mLayerConfig.get();
	}

	CSceneNode* ILayer::getSceneNode()
	{
		return mSceneNode.get();
	}

	void ILayer::setSceneNode(CSceneNode* node)
	{
		
	}

	void ILayer::setParent(ILayer* layer)
	{
		mParentLayer = layer;
	}

	ILayer* ILayer::getParent()
	{
		return mParentLayer.get();
	}

	void ILayer::loadAsync()
	{
	
	}
}


