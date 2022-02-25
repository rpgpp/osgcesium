#include "IModelDatabase.h"
#include "PipeTable.h"
#include "PipeModelFactory.h"

namespace OC
{
	namespace Modeling
	{
		IModelDatabase::IModelDatabase(void)
		{
		}

		IModelDatabase::~IModelDatabase(void)
		{
		}

		bool IModelDatabase::getData(IModelTable* table)
		{
			String tableName = table->getName();

			if (tableName.empty())
			{
				return false;
			}

			String tablename = table->getName();
			String tableType = table->getType();
			if ( tableType == IModelTable::TableUnderGroundPipe || tableType == IModelTable::TableOverGroundPipe)
			{
				PipeTable* pTable = static_cast<PipeTable*>(table);

				bool lineClause = pTable->getLineTable().getFilter().valid();
				bool nodeClause = pTable->getNodeTable().getFilter().valid();

				LineTable& lineTable = pTable->getLineTable();
				NodeTable& nodeTable = pTable->getNodeTable();

  				if (lineClause == nodeClause)
				{
					if (getLineData(pTable) == false)
					{
						return false;
					}
					if (getNodeData(pTable) == false)
					{
						return false;
					}
				}
				else if (lineClause)
				{
					
				}
				else if (nodeClause)
				{
				
				}

				pTable->fixData();

				return true;
			}
			else if (tableType == IModelTable::ShelfOverGround || tableType == IModelTable::ShelfUnderGround)
			{
				
			}
			return false;
		}

		void IModelDatabase::enumMetaTables()
		{
			osg::ref_ptr<OCHeaderInfo> header = getHeadInfo("M_DBMETA");
			if (header.valid())
			{
				TemplateRecordList list = query("M_DBMETA",header);
				TemplateRecordList::iterator it = list.begin();
				for (;it!=list.end();it++)
				{
					TemplateRecord* record = *it;
					String layername = record->getFieldValue(header->getFieldInfo(0)->Name);//0 is layername
					if (!layername.empty() && layername != "marker")
					{
						mMetaRecordMap[layername] = record;
					}
				}
			}
			else
			{
				osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo;
				header->add(new OCFieldInfo("layername",OC_DATATYPE_TEXT));
				StringMap sm;
				StringVector geomtry_layers = enumGeometryLayers();
				StringVector::iterator it = geomtry_layers.begin();
				for (;it!=geomtry_layers.end();it++)
				{
					String layername = *it;

					{
						StringVector sv = StringUtil::split(layername,"_");
						if (sv.size() == 2)
						{
							String table_net = sv[0];
							String layername2;
							if (PipeTableSplice + sv[1] == NodeTableSubfix)
							{
								layername2 = table_net + LineTableSubfix;
							}
							else if (PipeTableSplice + sv[1] == LineTableSubfix)
							{
								layername2 = table_net + NodeTableSubfix;
							}

							if (sm.find(layername2) == sm.end())
							{
								sm[layername] = layername;
							}
							else
							{
								osg::ref_ptr<TemplateRecord> record = new TemplateRecord(header);
								record->setFieldValue("layername",table_net);
								mMetaRecordMap[table_net] = record;
							}
						}
					}
				}
			}
		}

		MetaRecordMapIterator IModelDatabase::getMetaTableIterator()
		{
			return MetaRecordMapIterator(mMetaRecordMap.begin(),mMetaRecordMap.end());
		}

		TemplateRecord* IModelDatabase::findMetaTable(String layername)
		{
			MetaRecordMap::iterator it = mMetaRecordMap.find(layername);
			if (mMetaRecordMap.end() == it)
			{
				return NULL;
			}

			return it->second.get();
		}

		bool IModelDatabase::dynamic_build(CTileFeatureNode* tilefeaturenode)
		{
			
			return true;
		}

	}//namespace modeling
}
