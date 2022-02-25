#ifndef _LAYER_CONFIG_CLASS_H__
#define _LAYER_CONFIG_CLASS_H__

#include "OCMain/CoreConfig.h"
#include "LayerDefine.h"

namespace OC
{
	typedef std::vector<osg::ref_ptr<CLayerConfig> >		LayerConfigList;
	typedef std::map<String,osg::ref_ptr<CLayerConfig> > LayerConfigMap;
	typedef MapIterator<LayerConfigMap>						LayerConfigMapIt;

	class _LayerExport CLayerConfig : public osg::Referenced , public CCoreConfig
	{
	public:
		CLayerConfig();
		virtual ~CLayerConfig();
		CLayerConfig(const CLayerConfig &layerConfig);

		void setVisibleRange(CVector2 min_max);
		CVector2 getVisibleRange();

		virtual void fromXmlString(String xmlString);
		virtual String toXmlString();

		virtual void fromXmlElement(TiXmlElement* element);
		virtual TiXmlElement* toXmlElement(TiXmlElement* element = NULL);

		void fromJSON(String json);
		String toJSON(bool empty = false);

		CLayerConfig* findChild(String name);
		void insertChild(CLayerConfig* config);
		LayerConfigMapIt getChildConfigIterator();

		void writeXmlElement(TiXmlElement* e,String configName,CLayerConfig* layer);
		void readXmlElement(TiXmlElement* e,String configName,CLayerConfig* layer);

		static String NameID;
		static String NameTime;
		static String NameLink;
		static String NameColor;
		static String NameTable;
		static String NameOffset;
		static String NameEnable;
		static String NameCityLod;
		static String NameDisplay;
		static String NameVisible;
		static String NameReflect;
		static String NameTexture;
		static String NameLighting;
		static String NamePickable;
		static String NameRangeMax;
		static String NameRangeMid;
		static String NameRangeMin;
		static String NameRangeBox;
		static String NameIsGround;
		static String NamePriority;
		static String NameIsWater;
		static String NameIsShadowScene;
		static String NameIsReceiveShadow;
		static String NameIsCastShadow;
		static String NameResident;
		static String NameSceneType;
		static String NameRenderBin;
		static String NameLayerType;
		static String NamePolygonOffset;
		static String NameVisibleRegion;
		static String NameIsGeocenteric;
		static String NameJumpPosition; 
	private:
		LayerConfigMap	mChildConfigMap;
	};
}


#endif


