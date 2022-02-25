#pragma once

#include "OCModel/IModelDatabase.h"
#include "ogrsf_frmts.h"

namespace OC
{
	namespace Modeling
	{
		typedef std::map<String,OGRDataSourceH> SourceMap;

		class ShapeFileDatabase:public IModelDatabase
		{
		public:
			ShapeFileDatabase();
			~ShapeFileDatabase();
			virtual TemplateRecordList query(String table,OCHeaderInfo* headinfo,CQueryInfo* filter = NULL);
			virtual bool create(String url,String user = "",String psw = "");

			virtual bool connect(String connectStr);
			virtual OCHeaderInfo* getHeadInfo(String table);
			virtual StringVector enumGeometryLayers();
			virtual StringVector enumNets();
			virtual StringVector emunFields(String table,String hdr);
			virtual bool getLineData(PipeTable* table);
			virtual bool getNodeData(PipeTable* table);
			void initShpDriver();
			StringVector simplequery(String sqlStr);
			StringVector simplequery(String sqlStr,OGRDataSourceH datasrc);
			
			OGRDataSourceH open_shapefile(String file);
			bool initShpDriver(String strShpFile);
			bool writeFeature(FeatureRecord* record);
		private:
			void destroy();
			OGRSFDriverH			_ogrDriverHandle;
			String					mShpPath;
			SourceMap				mSourceMap;

			OGRDataSourceH			m_pDataSourceOut;
			OGRLayerH				m_pLayer;
			String					m_strShpFile;
		};
	}
}