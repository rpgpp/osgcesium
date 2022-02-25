#include "TableConfig.h"

#define TagCondition "condition"

namespace OC
{
	namespace Modeling
	{
		String TableConfig::TagTableConfig				= "table";
		String TableConfig::NameZFlag				= "Zflag";
		String TableConfig::NameTuber				= "tuber";
		String TableConfig::NameHasDianGanNode				= "hasDianXianGan";
		String TableConfig::NameHasXiaoFangShuanNode				= "hasXiaoFangShuan";
		String TableConfig::NameRegion				= "Region";
		String TableConfig::NameDisplayName			= "DisplayName";
		String TableConfig::NameName				= "name";
		String TableConfig::NameUseWSTexture		= "useWSTexture";
		String TableConfig::NameFixNullSectLine		= "FixNullSect";
		String TableConfig::NameBuildIsolatePoint	= "BuildIsolatePoint";
		String TableConfig::NameDirection			= "direction";  //流向字段
		String TableConfig::NameTimeStamp		= "TimeStamp";
		String TableConfig::NameConstantElevation		= "ConstantElevation";
		String TableConfig::NamePlusUnderGroundNode		= "PlusUnderGroundNode";
		String TableConfig::NameExtension		= "Extension";
		String TableConfig::NameBuildOnlySpatialIndex		= "BuildOnlySpatialIndex";
		String TableConfig::NameConnectString		= "ConnectString";
		String TableConfig::NameOptimizeLevel		= "Level";
		String TableConfig::NameNodeClause		= "NodeClause";
		String TableConfig::NameLineClause		= "LineClause";
		String TableConfig::NameMaterialColor	= "MaterialColor";
		String TableConfig::NameDefaultSect	= "DefaultSect";
		String TableConfig::NameEdition	= "edition";
		String TableConfig::NameVersion	= "version";
		TableConfig::TableConfig(void)
			:mXmlElement(NULL)
		{
			setConfig(NameExtension,String("pipe"));
		}

		TableConfig::TableConfig(const TableConfig &config)
			:CLayerConfig(config)
		{
			mModelConfig = config.mModelConfig;
			mFieldMapping= config.mFieldMapping;
			mXmlElement= config.mXmlElement;
		}

		TableConfig::~TableConfig(void)
		{
		}

		MeasureZ_Flag TableConfig::getMeasureZ_Flag()
		{
			String flag = getStringValue(NameZFlag);

			if (flag == "1" || flag == "管底" || flag == "管底标高")
			{
				return MeasureZ_IS_BOTTOM;
			}
			else if (flag == "2" || flag == "管顶" || flag == "管顶标高")
			{
				return MeasureZ_IS_TOP;
			}

			return MeasureZ_IS_CENTER;
		}
		
		void TableConfig::setMeasureZ_Flag(MeasureZ_Flag flag)
		{
			switch(flag)
			{
			case MeasureZ_IS_BOTTOM:
				{
					setConfig(NameZFlag,OC::String("1"));
					break;
				}
			case MeasureZ_IS_TOP:
				{
					setConfig(NameZFlag,OC::String("2"));
					break;
				}
			case MeasureZ_IS_CENTER:
				{
					setConfig(NameZFlag,OC::String(""));
					break;
				}				
			}
		}

		void TableConfig::fromXmlElement(TiXmlElement* element,ModelConfigMap&	globalModelSource)
		{
			mXmlElement = element;

			String name = TinyXml::read_attribute(element,TableConfig::NameDisplay);
			setConfig(TableConfig::NameDisplay,name);
			setConfig(TableConfig::NameColor,TinyXml::read_attribute(element,TableConfig::NameColor));
			setConfig(TableConfig::NameTuber,TinyXml::read_attribute(element,TableConfig::NameTuber));
			setConfig(TableConfig::NameZFlag,TinyXml::read_attribute(element,TableConfig::NameZFlag));
			setConfig(TableConfig::NameDisplayName,TinyXml::read_attribute(element,TableConfig::NameDisplayName));
			setConfig(TableConfig::NameUseWSTexture,TinyXml::read_attribute(element,TableConfig::NameUseWSTexture));
			setConfig(TableConfig::NameMaterialColor,TinyXml::read_attribute(element,TableConfig::NameMaterialColor));
			setConfig(TableConfig::NameDefaultSect,TinyXml::read_attribute(element,TableConfig::NameDefaultSect));

			//condition
			for(TiXmlElement *condition_element = element->FirstChildElement(TagCondition);condition_element;condition_element = condition_element->NextSiblingElement(TagCondition))
			{
				String modelName = TinyXml::read_attribute(condition_element,NameName);
				String condition = condition_element->GetText(); 
				ModelConfigMap::iterator it = globalModelSource.find(modelName); 
				if (it != globalModelSource.end())
				{
					addModelMapped(condition,it->second);
				}
			}
		}

		TiXmlElement* TableConfig::toXmlElement(TiXmlElement* element)
		{
			if (element == NULL)
			{
				element = new TiXmlElement(TagTableConfig);
			}

			writeXmlElement(element,TableConfig::NameDisplay,this);
			writeXmlElement(element,TableConfig::NameColor,this);
			writeXmlElement(element,TableConfig::NameTuber,this);
			writeXmlElement(element,TableConfig::NameZFlag,this);
			writeXmlElement(element,TableConfig::NameDisplayName,this);
			writeXmlElement(element,TableConfig::NameUseWSTexture,this);
			writeXmlElement(element,TableConfig::NameMaterialColor,this);
			writeXmlElement(element,TableConfig::NameDefaultSect,this);

			while(TiXmlElement* elem = element->FirstChildElement(TagCondition))
			{
				element->RemoveChild(elem);
			}

			ModelConfigIterator it = getModelIterator();
			while(it.hasMoreElements())
			{
				String k = it.peekNextKey();
				ModelNode n = it.getNext();

				TiXmlElement* elem = new TiXmlElement(TagCondition);
				elem->SetAttribute("name",n.getName());
				
				TiXmlText* txt = new TiXmlText(TagCondition);
				txt->SetValue(k);
				elem->LinkEndChild(txt);
				element->LinkEndChild(elem);
			}


			return element; 
		}

		FieldMapping* TableConfig::getFieldMapping()
		{
			return mFieldMapping.get(); 
		}

		void TableConfig::setFieldMapping(FieldMapping* fm)
		{
			mFieldMapping = fm;
		}

		bool TableConfig::isConfigModel(String fieldVal)
		{
			return mModelConfig.find(fieldVal)!=mModelConfig.end();
		}

		ModelConfigIterator TableConfig::getModelIterator()
		{
			return ModelConfigIterator(mModelConfig.begin(),mModelConfig.end());
		}

		ModelNode TableConfig::getConfigModel(String fieldVal)
		{
			return mModelConfig[fieldVal];
		}

		String TableConfig::getDisplayName()
		{
			String displayName = getStringValue(NameDisplayName);
			if (!displayName.empty())
			{
				return displayName;
			}
			return getStringValue(NameDisplay);
		}

		void TableConfig::clearModelMapping()
		{
			mModelConfig.clear();
			setConfig(NameHasDianGanNode,false);
			setConfig(NameHasXiaoFangShuanNode,false);
		}

		void TableConfig::addModelMapped(String fieldvalue,ModelNode model)
		{
			mModelConfig[fieldvalue] = model;

			String name = model.getName();
			if (name == "电杆")
			{
				setConfig(NameHasDianGanNode,true);
			}
			if (name == "消防栓" || name == "消防栓7")
			{
				setConfig(NameHasXiaoFangShuanNode,true);
			}
		}

	}
}
