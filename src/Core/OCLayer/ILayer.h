#ifndef _ILAYER_CLASS_
#define _ILAYER_CLASS_

#include "LayerConfig.h"
#include "LayerDefine.h"

namespace OC
{	
	typedef std::vector<osg::ref_ptr<ILayer> >			LayerList;
	typedef std::map<String,osg::ref_ptr<ILayer> >		LayerMap;
	typedef MapIterator<LayerMap>						LayerMapIterator;
	typedef std::map<String,osg::observer_ptr<ILayer> >	ObserverLayerMap;
	typedef MapIterator<ObserverLayerMap >				ObserverLayerMapIterator;

	class _LayerExport ILayer :public osg::Referenced
	{
	public:
		ILayer(LayerManager* creator,String ID);
		virtual ~ILayer(void);

		virtual void setVisible(bool visible);
		virtual bool getVisible();

		void setName(String name);
		String getName();

		void setID(String id);
		String getID();

		void setURL(String url);
		String getURL();

		virtual bool load(){return false;}
		virtual void loadAsync();
		virtual void onLoadComplete(){}
		virtual void unload(){}
		virtual void reload(){}
		virtual void attach(){}
		virtual void detach(){}
		virtual void refresh(){}

		virtual CFeatureLayer*		asFeatureLayer(){return NULL;}

		CLayerConfig& getConfig();
		void setConfig(CLayerConfig* config);

		CSceneNode* getSceneNode();
		void setSceneNode(CSceneNode* node);

		void setParent(ILayer* layer);
		ILayer* getParent();
	protected:
		bool								mVisible;
		bool								mLoaded;
		String								mID;
		String								mURL;
		String								mName;
		String								mTable;
		LayerManager*						mCreator;
		osg::ref_ptr<CSceneNode>			mSceneNode;
		osg::ref_ptr<CLayerConfig>			mLayerConfig;
		osg::observer_ptr<ILayer>			mParentLayer;
		CORE_MUTEX(LayerMutex)
	};
}

#endif