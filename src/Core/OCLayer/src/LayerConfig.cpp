#include "LayerConfig.h"
#include "OCMain/JsonPackage.h"

namespace OC
{
	String CLayerConfig::NameID = "id";
	String CLayerConfig::NameTime = "time";
	String CLayerConfig::NameLink = "link";
	String CLayerConfig::NameEnable = "enable";
	String CLayerConfig::NameDisplay = "name";
	String CLayerConfig::NameColor = "color";
	String CLayerConfig::NameTable = "table";
	String CLayerConfig::NameOffset = "Offset";
	String CLayerConfig::NameCityLod = "CityLod";
	String CLayerConfig::NameVisible = "visible";
	String CLayerConfig::NameTexture = "Texture";
	String CLayerConfig::NameReflect = "Reflect";
	String CLayerConfig::NameLighting = "Lighting";
	String CLayerConfig::NamePickable = "PickMode";
	String CLayerConfig::NameRenderBin = "RenderBin";
	String CLayerConfig::NameRangeMax = "RangeMax";
	String CLayerConfig::NameRangeMid = "RangeMid";
	String CLayerConfig::NameRangeMin = "RangeMin";
	String CLayerConfig::NameRangeBox = "RangeBox";
	String CLayerConfig::NameIsGround = "IsGround";
	String CLayerConfig::NamePriority = "Priority";
	String CLayerConfig::NameIsWater = "IsWater";
	String CLayerConfig::NameResident = "Resident";
	String CLayerConfig::NameSceneType = "SceneType";
	String CLayerConfig::NameLayerType = "LayerType";
	String CLayerConfig::NameVisibleRegion = "VisibleRegion";
	String CLayerConfig::NameIsGeocenteric = "IsGeocentric";
	String CLayerConfig::NamePolygonOffset = "PolygonOffset";
	String CLayerConfig::NameIsShadowScene = "IsShadowScene";	
	String CLayerConfig::NameIsReceiveShadow = "IsReceiveShadow";
	String CLayerConfig::NameIsCastShadow = "IsCastShadow";
	String CLayerConfig::NameJumpPosition = "JumpPosition";

	CLayerConfig::CLayerConfig()
	{
	}

	CLayerConfig::~CLayerConfig()
	{
	}

	CLayerConfig::CLayerConfig(const CLayerConfig &layerConfig)
		:CCoreConfig(layerConfig)
	{
		
	}

	void CLayerConfig::writeXmlElement(TiXmlElement* e,String configName,CLayerConfig* layer)
	{
		String cfgVal = layer->getStringValue(configName);
		if (!cfgVal.empty())
		{
			e->ToElement()->SetAttribute(configName,cfgVal);
		}
	}

	void CLayerConfig::readXmlElement(TiXmlElement* e,String configName,CLayerConfig* layer)
	{
		const String* cfgVal = e->Attribute(configName);
		if (cfgVal)
		{
			layer->setConfig(configName,*cfgVal);
		}
	}

	inline void addJson(JsonPackage& package,bool empty,String name,CCoreConfig* config)
	{
		String cfgVal = config->getStringValue(name);
		if (!cfgVal.empty() || empty)
		{
			package.addNameVal(name,cfgVal);
		}
	}

	inline void readJson(OC::Json::Value& root,String name,CLayerConfig* config)
	{
		String cfgVal = JsonPackage::getString(root,name);
		if (!cfgVal.empty())
		{
			config->setConfig(name,cfgVal);
		}
	}

	void CLayerConfig::fromJSON(String json)
	{
		try
		{
			OC::Json::Reader reader;
			OC::Json::Value root;
			if(reader.parse(json,root))
			{
				readJson(root,NameID,this);
				readJson(root,NameTime,this);
				readJson(root,NameColor,this);
				readJson(root,NameTable,this);
				readJson(root,NameEnable,this);
				readJson(root,NameOffset,this);
				readJson(root,NamePickable,this);
				readJson(root,NameTexture,this);
				readJson(root,NameReflect,this);
				readJson(root,NameLighting,this);
				readJson(root,NameVisible,this);
				readJson(root,NameDisplay,this);
				readJson(root,NameRangeMin,this);
				readJson(root,NameRangeMid,this);
				readJson(root,NameRangeMax,this);
				readJson(root,NameIsGround,this);
				readJson(root,NameResident,this);
				readJson(root,NamePriority,this);
				readJson(root,NameIsWater,this);
				readJson(root,NameRenderBin,this);
				readJson(root,NameLayerType,this);
				readJson(root,NameIsGeocenteric,this);
				readJson(root,NamePolygonOffset,this);
				readJson(root,NameIsCastShadow,this);
				readJson(root,NameJumpPosition,this);
				readJson(root,NameIsReceiveShadow,this);
				readJson(root, NameIsShadowScene, this); 
				readJson(root, NameCityLod, this);
			}
		}
		catch (...){}
	}

	String CLayerConfig::toJSON(bool empty)
	{
		JsonPackage package;

		addJson(package,empty,NameID,this);
		addJson(package,empty,NameTime,this);
		addJson(package,empty,NameColor,this);
		addJson(package,empty,NameTable,this);
		addJson(package,empty,NameEnable,this);
		addJson(package,empty,NameOffset,this);
		addJson(package,empty,NamePickable,this);
		addJson(package,empty,NameTexture,this);
		addJson(package,empty,NameReflect,this);
		addJson(package,empty,NameLighting,this);
		addJson(package,empty,NameVisible,this);
		addJson(package,empty,NameDisplay,this);
		addJson(package,empty,NameRangeMin,this);
		addJson(package,empty,NameRangeMid,this);
		addJson(package,empty,NameRangeMax,this);
		addJson(package,empty,NameIsGround,this);
		addJson(package,empty,NameResident,this);
		addJson(package,empty,NamePriority,this);
		addJson(package,empty,NameIsWater,this);
		addJson(package,empty,NameRenderBin,this);
		addJson(package,empty,NameLayerType,this);
		addJson(package,empty,NameIsGeocenteric,this);
		addJson(package,empty,NamePolygonOffset,this);
		addJson(package,empty,NameIsCastShadow,this);
		addJson(package,empty,NameJumpPosition,this);
		addJson(package,empty,NameIsReceiveShadow,this);
		addJson(package, empty, NameIsShadowScene, this);
		addJson(package, empty, NameCityLod, this);
		return package.toJSON();
	}

	TiXmlElement* CLayerConfig::toXmlElement(TiXmlElement* element)
	{
		String layerType = getStringValue(CLayerConfig::NameLayerType);

		if(element == NULL)
		{
			element= new TiXmlElement(layerType);
		}

		if (layerType == OC_LAYER_GROUP)
		{
			writeXmlElement(element,NameLink,this);
		}
		else
		{	
			if(TiXmlElement* linkElement = element->FirstChildElement(NameLink))
			{
				TiXmlText *textElement = linkElement->FirstChild()->ToText();
				if(textElement != NULL)
				{
					textElement->SetValue(getStringValue(CLayerConfig::NameLink));
				}
			}
			else			
			{
				TiXmlElement* linkElem = new TiXmlElement(NameLink);
				TiXmlText* text = new TiXmlText(NameLink);
				text->SetValue(getStringValue(CLayerConfig::NameLink));
				linkElem->LinkEndChild(text);
				element->LinkEndChild(linkElem);
			}			
		}

		writeXmlElement(element,NameID,this);
		writeXmlElement(element,NameTime,this);
		writeXmlElement(element,NameColor,this);
		writeXmlElement(element,NameTable,this);
		writeXmlElement(element,NameEnable,this);
		writeXmlElement(element,NameOffset,this);
		writeXmlElement(element,NamePickable,this);
		writeXmlElement(element,NameTexture,this);
		writeXmlElement(element,NameReflect,this);
		writeXmlElement(element,NameLighting,this);
		writeXmlElement(element,NameVisible,this);
		writeXmlElement(element,NameDisplay,this);
		writeXmlElement(element,NameRangeMin,this);
		writeXmlElement(element,NameRangeMid,this);
		writeXmlElement(element,NameRangeMax,this);
		writeXmlElement(element,NameIsGround,this);
		writeXmlElement(element,NameResident,this);
		writeXmlElement(element,NamePriority,this);
		writeXmlElement(element,NameIsWater,this);
		writeXmlElement(element,NameRenderBin,this);
		writeXmlElement(element,NameIsGeocenteric,this);
		writeXmlElement(element,NamePolygonOffset,this);
		writeXmlElement(element,NameSceneType,this);
		writeXmlElement(element,NameIsCastShadow,this);
		writeXmlElement(element,NameJumpPosition,this);
		writeXmlElement(element,NameIsReceiveShadow,this);
		writeXmlElement(element, NameIsShadowScene, this);
		writeXmlElement(element, NameCityLod, this);
		return element;
	}

	void CLayerConfig::fromXmlElement(TiXmlElement* element)
	{
		try
		{
			readXmlElement(element,NameID,this);
			readXmlElement(element,NameLink,this);
			readXmlElement(element,NameTime,this);
			readXmlElement(element,NameColor,this);
			readXmlElement(element,NameTable,this);
			readXmlElement(element,NameEnable,this);
			readXmlElement(element,NameOffset,this);
			readXmlElement(element,NamePickable,this);
			readXmlElement(element,NameTexture,this);
			readXmlElement(element,NameReflect,this);
			readXmlElement(element,NameLighting,this);
			readXmlElement(element,NameVisible,this);
			readXmlElement(element,NameDisplay,this);
			readXmlElement(element,NameRangeMin,this);
			readXmlElement(element,NameRangeMid,this);
			readXmlElement(element,NameRangeMax,this);
			readXmlElement(element,NameIsGround,this);
			readXmlElement(element,NameResident,this);
			readXmlElement(element,NamePriority,this);
			readXmlElement(element,NameIsWater,this);
			readXmlElement(element,NameSceneType,this);
			readXmlElement(element,NameRenderBin,this);
			readXmlElement(element,NameIsGeocenteric,this);
			readXmlElement(element,NamePolygonOffset,this);
			readXmlElement(element,NameIsShadowScene,this);
			readXmlElement(element,NameIsReceiveShadow,this);
			readXmlElement(element,NameIsCastShadow,this);
			readXmlElement(element,NameJumpPosition,this);
			readXmlElement(element,"Cartoon",this);
			readXmlElement(element, "Rotate", this);
			readXmlElement(element, NameCityLod, this);
			if (TiXmlElement* fe = element->FirstChildElement())
			{
				try
				{
					String head = String(fe->Value());
					StringUtil::toLowerCase(head);
					if (head == NameLink)
					{
						if (const char* link = fe->GetText())
						{
							setConfig(NameLink,String(link));
						}
					}
				}
				catch (...){}
			}
		}
		catch (...){}
	}

	void CLayerConfig::fromXmlString(String xmlString)
	{
		if (xmlString.empty())
		{
			return;
		}

		TiXmlDocument doc;
		try
		{
			doc.Parse(xmlString.c_str());
			TiXmlElement* item = doc.FirstChildElement(OC_LAYER_FEATURE);
			if (item)
			{
				fromXmlElement(item);
			}
		}
		catch (...){}
		setConfig(CLayerConfig::NameLayerType,String(OC_LAYER_FEATURE));
	}

	void CLayerConfig::setVisibleRange(CVector2 min_max)
	{
		setConfig(NameRangeMin,min_max.x);
		setConfig(NameRangeMax,min_max.y);
	}

	CVector2 CLayerConfig::getVisibleRange()
	{
		CVector2 min_max;
		min_max.x = Math::Abs(getIntValue(NameRangeMin));

		min_max.y = getDoubleValue(NameRangeMax,0.0f);
		if (min_max.y < 1.0f)
		{
			min_max.y = Singleton(ConfigManager).getDefaultVisibleDistance();
		}

		return min_max;
	}

	String CLayerConfig::toXmlString()
	{
		String xmlString;
		try
		{
			TiXmlDocument doc;
			doc.LinkEndChild(toXmlElement());  
			TiXmlPrinter printer;
			doc.Accept(&printer);
			xmlString = printer.CStr();
		}
		catch (...)
		{}

		return xmlString;
	}

	CLayerConfig* CLayerConfig::findChild(String name)
	{
		LayerConfigMap::iterator it = mChildConfigMap.find(name);
		if (it!=mChildConfigMap.end())
		{
			return it->second;
		}
		return NULL;
	}

	void CLayerConfig::insertChild(CLayerConfig* config)		
	{
		String key = config->getStringValue(NameDisplay);
		mChildConfigMap[key] = config;
	}

	LayerConfigMapIt CLayerConfig::getChildConfigIterator()	
	{
		return LayerConfigMapIt(mChildConfigMap.begin(),mChildConfigMap.end());
	}

}