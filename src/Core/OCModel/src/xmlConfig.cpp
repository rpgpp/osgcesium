#include "xmlConfig.h"
#include "IModelTable.h"

#define DEFAULT_TABLE "Default"

namespace OC
{
	namespace Modeling
	{
		CXmlConfig::CXmlConfig(void)
			:m_load(false)
		{

		}

		CXmlConfig::~CXmlConfig(void)
		{

		}

		ModelConfigMap CXmlConfig::getModelConfigMap()
		{
			return mModelMap;
		}

		void CXmlConfig::setDefaultFieldMapping(String fieldmapping)
		{
			mDefaultFieldMapping = fieldmapping;
		}

		String CXmlConfig::getDefaultFieldMapping()
		{
			return mDefaultFieldMapping;
		}

		FieldMappingVectorIterator CXmlConfig::getFieldMappingIterator()
		{
			return FieldMappingVectorIterator(mFieldMappingList.begin(),mFieldMappingList.end());
		}

		TableVectorIterator CXmlConfig::getTableVectorIterator()
		{
			return TableVectorIterator(mTableConfigList.begin(),mTableConfigList.end());
		}

		void CXmlConfig::decorate(IModelTable* table)
		{
			String name = table->getName();

			TableConfig cfg;

			TableConfigMap::iterator it = mTableConfigMap.find(name);
			if (it != mTableConfigMap.end())
			{
				cfg = *it->second;
			}
			else
			{
				cfg = *mTableConfigMap[DEFAULT_TABLE];
			}

			if (FieldMapping* fm = findFieldMapping(getDefaultFieldMapping()))
			{
				cfg.setFieldMapping(fm);
			}

			table->setConfig(cfg);
		}

		void CXmlConfig::removeTable(TableConfig* config)
		{
			TiXmlElement* element = config->getXmlElement();
			if (element)
			{
				if (TiXmlNode* p = element->Parent())
				{
					p->RemoveChild(element);
				}
			}

			TableConfigList::iterator it = std::find(mTableConfigList.begin(),mTableConfigList.end(),config);
			if (it!=mTableConfigList.end())
			{
				mTableConfigList.erase(it);
			}

			mTableConfigMap.erase(mTableConfigMap.find(config->getStringValue(TableConfig::NameName)));
		}

		void CXmlConfig::addTable(TableConfig* config)
		{
			String name = config->getStringValue(TableConfig::NameName);
			if (findTable(name) == NULL)
			{
				mTableConfigMap[name] = config;
				mTableConfigList.push_back(config);
			}
		}

		void CXmlConfig::changeFieldMappingName(String old,String name)
		{
			osg::ref_ptr<FieldMapping> fm;

			FieldMappingMap::iterator it = mFieldMappingMap.find(old);
			if (it!=mFieldMappingMap.end())
			{
				fm = it->second;
				mFieldMappingMap.erase(it);
			}

			if (fm.valid())
			{
				fm->setName(name);
				mFieldMappingMap[name] = fm;
				fm->setDirty();
			}
		}

		void CXmlConfig::addFieldMapping(FieldMapping* fm)
		{
			String name = fm->getName();
			if (findTable(name) == NULL)
			{
				mFieldMappingMap[name] = fm;
				mFieldMappingList.push_back(fm);
			}
		}

		FieldMapping* CXmlConfig::findFieldMapping(String map_name)
		{
			FieldMappingMap::iterator it = mFieldMappingMap.find(map_name);
			if (it != mFieldMappingMap.end())
			{
				return it->second.get();
			}
			else
			{
				return NULL;
			}
		}


		TableConfig* CXmlConfig::findTable(String table_name)
		{
			TableConfigMap::iterator it = mTableConfigMap.find(table_name);
			if (it != mTableConfigMap.end())
			{
				return it->second.get();
			}
			else
			{
				return NULL;
			}
		}

		void CXmlConfig::save()
		{
			try
			{
				if (TiXmlElement *root = mDocument.RootElement())
				{
					TableVectorIterator it = getTableVectorIterator();
					while(it.hasMoreElements())
					{
						TableConfig* config = it.getNext();

						TiXmlElement* element = config->toXmlElement(config->getXmlElement());
						if (element != config->getXmlElement())
						{
							if (root->InsertAfterChild(root->LastChild(TableConfig::TagTableConfig),*element))
							{
								CORE_SAFE_DELETE(element);
							}
							else
							{
								root->LinkEndChild(element);
							}
						}
					}

					for (TiXmlElement* element = root->FirstChildElement(FieldMapping::TagFieldMap);element;element=element->NextSiblingElement(FieldMapping::TagFieldMap))
					{
						String name = TinyXml::read_attribute(element,"name");
						element->SetAttribute("enable",name == mDefaultFieldMapping ? "true" : "false");
						if (FieldMapping* fm = findFieldMapping(name))
						{
							if (fm->isDirty())
							{
								fm->toXmlElement(element);
							}
						}
					}

					mDocument.SaveFile();
				}
			}
			catch (...)
			{}
		}

		bool CXmlConfig::loadXml(String xmlPath)
		{
			Singleton(LogManager).stream(LML_NORMAL)<<"读取配置文件:"<<xmlPath;

			mTableConfigMap.clear();
			mTableConfigList.clear();
			
			try
			{

				if(mDocument.LoadFile(xmlPath.c_str()))
				{
					mXmlPath = xmlPath;

					TiXmlElement *root = mDocument.RootElement();

					//1.读取模型库				
					for(TiXmlNode *modelSrcNode = root->FirstChild("ModelSource");modelSrcNode;modelSrcNode = modelSrcNode->NextSibling("ModelSource"))
					{
						for(TiXmlElement *model_element = modelSrcNode->FirstChildElement("Model");model_element;model_element = model_element->NextSiblingElement("Model"))
						{
							ModelNode model;
							model.fromXmlElement(model_element);
							mModelMap[model.getName()] = model;
						}
					}

					osg::ref_ptr<FieldMapping> defaultFM;

					int fm_c = 1;
					//2.取数据规则
					for(TiXmlElement *fieldmapping_element = root->FirstChildElement(FieldMapping::TagFieldMap);
						fieldmapping_element;fieldmapping_element = fieldmapping_element->NextSiblingElement(FieldMapping::TagFieldMap))
					{
						String enable = TinyXml::read_attribute(fieldmapping_element->ToElement(),"enable");

						osg::ref_ptr<FieldMapping> fieldmapping = new FieldMapping;
						fieldmapping->fromXmlElement(fieldmapping_element);

						String fmn = TinyXml::read_attribute(fieldmapping_element->ToElement(),"name");
						if (fmn.empty())
						{
							fmn = "规则__" + StringConverter::toString(fm_c++);
						}

						if (StringConverter::parseBool(enable) == true)
						{								
							defaultFM = fieldmapping;
							mDefaultFieldMapping = fmn;
						}
						fieldmapping->setName(fmn);
						addFieldMapping(fieldmapping);
					}

					//3.读取每个表的建表规则
					for(TiXmlElement* table_element = root->FirstChildElement(TableConfig::TagTableConfig);table_element;table_element = table_element->NextSiblingElement(TableConfig::TagTableConfig))
					{	
						osg::ref_ptr<TableConfig> config = new TableConfig;
						config->setFieldMapping(defaultFM);
						config->fromXmlElement(table_element,mModelMap);
						addTable(config);
					}

					//set default
					if (!mTableConfigMap[DEFAULT_TABLE].valid())
					{
						osg::ref_ptr<TableConfig> config = new TableConfig;
						config->setConfig(TableConfig::NameName,DEFAULT_TABLE);
						addTable(config);
 					}
					mTableConfigMap[DEFAULT_TABLE]->setFieldMapping(defaultFM);
					return m_load = true;
				}
			}
			catch (...)
			{}
		
			return false;
		}

	}
}

