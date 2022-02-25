#ifndef _IMODEL_DATABASE_CALSS_H_
#define _IMODEL_DATABASE_CALSS_H_

#include "OCMain/osg.h"
#include "OCLayer/IDatabase.h"
#include "IModelTable.h"
#include "ModelDataQueryCallback.h"

namespace OC
{
	namespace Modeling
	{
		typedef std::map<String,osg::ref_ptr<TemplateRecord> > MetaRecordMap;
		typedef MapIterator<MetaRecordMap> MetaRecordMapIterator;

		class _ModelExport IModelDatabase : public IDatabase
		{
		public:
			IModelDatabase(void);
			virtual ~IModelDatabase(void);

			virtual StringVector enumNets(){return StringVector();}
			virtual CCoreConfig queryNetMeta(String layerName){ return CCoreConfig();}
			virtual StringVector emunFields(String table,String hdr){return StringVector();}

			bool getData(IModelTable* table);

			virtual bool getLineData(PipeTable* table){return false;}
			virtual bool getNodeData(PipeTable* table){return false;}
			virtual bool getShelfData(GenericMoldTable* table){return false;}

			virtual bool getSubLineData(CSubLineShelfTable* table){return false;}
			virtual bool getSubNodeData(CSubLineShelfTable* table){return false;}

			virtual String getDatabaseTime(){return StringUtil::BLANK;}

			virtual bool dynamic_build(CTileFeatureNode* tilefeaturenode);

			virtual StringVector enumGeometryLayers(){return StringVector();}
			virtual void enumMetaTables();
			TemplateRecord* findMetaTable(String layername);
			MetaRecordMapIterator getMetaTableIterator();

		protected:
			MetaRecordMap mMetaRecordMap;
		};
	}
}


#endif

