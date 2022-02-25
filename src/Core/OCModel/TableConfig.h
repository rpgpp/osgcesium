#ifndef _TABLE_CONFIG_CLASS_H_
#define _TABLE_CONFIG_CLASS_H_
#include "OCLayer/LayerConfig.h"
#include "Modelnode.h"
#include "DataFilter.h"
#include "FieldMapping.h"
#include "FieldMapping.h"

namespace OC
{
	namespace Modeling
	{
		typedef std::map<String,osg::ref_ptr<TableConfig> >	TableConfigMap; 
		typedef MapIterator<TableConfigMap> TableMapIterator;

		typedef std::vector<osg::ref_ptr<TableConfig> > TableConfigList;
		typedef VectorIterator<TableConfigList> TableVectorIterator;

		class _ModelExport TableConfig : public CLayerConfig
		{
		public:
			TableConfig();
			~TableConfig();
			TableConfig(const TableConfig &config);
			static String TagTableConfig;
			String getDisplayName();

			//Model
			bool isConfigModel(String fieldVal);
			ModelConfigIterator getModelIterator();
			ModelNode getConfigModel(String fieldVal);
			void clearModelMapping();
			void addModelMapped(String fieldvalue,ModelNode model);

			//Measure
			MeasureZ_Flag getMeasureZ_Flag();
			void setMeasureZ_Flag(MeasureZ_Flag flag);

			TiXmlElement* getXmlElement(){return mXmlElement;}
			void fromXmlElement(TiXmlElement* element,ModelConfigMap&	globalModelSource);
			virtual TiXmlElement* toXmlElement(TiXmlElement* element = NULL);

			//field mapping
			FieldMapping* getFieldMapping();
			void setFieldMapping(FieldMapping* fm);

			static String				NameName;
			static String				NameZFlag;
			static String				NameTuber;
			static String				NameHasXiaoFangShuanNode;
			static String				NameRegion;
			static String				NameHasDianGanNode;
			static String				NameDisplayName;
			static String				NameUseWSTexture;
			static String				NameFixNullSectLine;
			static String				NameBuildIsolatePoint;
			static String				NameDirection;
			static String				NameTimeStamp;
			static String				NameConstantElevation;
			static String				NamePlusUnderGroundNode;
			static String				NameExtension;
			static String				NameBuildOnlySpatialIndex;
			static String				NameConnectString;
			static String				NameOptimizeLevel;
			static String				NameNodeClause;
			static String				NameLineClause;
			static String				NameMaterialColor;
			static String				NameDefaultSect;
			static String				NameEdition;
			static String				NameVersion;
		protected:
			osg::ref_ptr<FieldMapping>			mFieldMapping;
			ModelConfigMap		mModelConfig;
			TiXmlElement*			mXmlElement;
		};
	}
}

#endif


