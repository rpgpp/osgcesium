#ifndef _OC_MODEL_XML_CONFIG_H__
#define _OC_MODEL_XML_CONFIG_H__

#include "TableConfig.h"
#include "FieldMapping.h"

namespace OC
{
	namespace Modeling
	{		
		class _ModelExport CXmlConfig
		{
		public:
			CXmlConfig(void);
			~CXmlConfig(void);

			bool loadXml(String path);
			void save();
			void decorate(IModelTable* table);

			//table config
			void removeTable(TableConfig* config);
			void addTable(TableConfig* config);
			TableConfig* findTable(String table_name);
			TableVectorIterator getTableVectorIterator();

			//fieldmapping
			void changeFieldMappingName(String old,String name);
			void addFieldMapping(FieldMapping* fm);
			FieldMapping* findFieldMapping(String map_name);
			FieldMappingVectorIterator getFieldMappingIterator();
			void setDefaultFieldMapping(String fieldmapping);
			String getDefaultFieldMapping();

			ModelConfigMap getModelConfigMap();


		protected:
			String										mDefaultFieldMapping;
			TableConfigList							mTableConfigList;
			TableConfigMap						mTableConfigMap;
			FieldMappingList						mFieldMappingList;
			FieldMappingMap						mFieldMappingMap;
			bool											m_load;
			String										mXmlPath;
			TiXmlDocument							mDocument;
			ModelConfigMap	mModelMap;
		};
	}
}

#endif
