#include "ShapeFileDatabase.h"
#include "OCModel/PipeTable.h"
#include "OCModel/ModelUtility.h"
#include <osgEarth/OgrUtils>

#undef OSGEARTHFEATURES_EXPORT

namespace OC
{
	namespace Modeling
	{
		ShapeFileDatabase::ShapeFileDatabase()
			:_ogrDriverHandle(NULL)
			,m_pDataSourceOut(NULL)
			,m_pLayer(NULL)
		{
			
		}

		ShapeFileDatabase::~ShapeFileDatabase()
		{
			destroy();
		}


		void ShapeFileDatabase::destroy()
		{
			SourceMap::iterator it = mSourceMap.begin();
			for(;it!=mSourceMap.end();it++)
			{
				if (it->second)
				{
					OGRReleaseDataSource(it->second);
				}
			}
			mSourceMap.clear();
			if (m_pDataSourceOut)
			{
				OGRReleaseDataSource(m_pDataSourceOut);
				m_pDataSourceOut = NULL;
			}
		}

		// helper function.
		OGRLayerH openLayer(OGRDataSourceH ds, const std::string& layer)
		{
			unsigned index = osgEarth::as<unsigned>(layer, 0);
			OGRLayerH h = OGR_DS_GetLayer(ds, index);
			return h;
		}

		void ShapeFileDatabase::initShpDriver()
		{
			if (_ogrDriverHandle == NULL)
			{
				OGRRegisterAll();
				//String s = CPLGetConfigOption("SHAPE_ENCODING","");
				//CPLSetConfigOption("SHAPE_ENCODING", "GBK2312");
				CPLSetConfigOption("SHAPE_ENCODING", "");
				CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");
				_ogrDriverHandle = OGRGetDriverByName("ESRI Shapefile");
			}
		}

		bool ShapeFileDatabase::connect(String connectStr)
		{
			mShpPath = osgDB::convertFileNameToNativeStyle(connectStr);
			StringUtil::toLowerCase(mShpPath);

			initShpDriver();

			if (_ogrDriverHandle == NULL)
			{
				return false;
			}

			if (FileUtil::FileExist(mShpPath))
			{
				OGRDataSourceH mDsHandle = OGROpen(A2U8(mShpPath).c_str(), 0, &_ogrDriverHandle);
				if (mDsHandle)
				{
					mSourceMap[mShpPath] = mDsHandle;
				}
			}

			return true;
		}

		bool ShapeFileDatabase::initShpDriver(String strShpFile)
		{
			m_strShpFile = A2U8(strShpFile);

			initShpDriver();

			if (_ogrDriverHandle == NULL)
			{
				return false;
			}

			m_pDataSourceOut = OGR_Dr_CreateDataSource(_ogrDriverHandle, m_strShpFile.c_str(),NULL);
			if (m_pDataSourceOut == NULL)
			{
				return false;
			}
			
			std::string strLayerName = m_strShpFile.substr(m_strShpFile.find_last_of("/") + 1);
			strLayerName = strLayerName.substr(0, strLayerName.find_last_of("."));

			m_pLayer = OGR_DS_CreateLayer(m_pDataSourceOut, A2U8(strLayerName).c_str(), NULL, wkbPoint, NULL);
			if (m_pLayer == NULL)
			{
				return false;
			}

			OGRFieldDefnH xField = OGR_Fld_Create("x", OFTReal);
			OGRFieldDefnH yField = OGR_Fld_Create("y", OFTReal);
			OGRFieldDefnH zField = OGR_Fld_Create("z", OFTReal);
			OGRFieldDefnH urlField = OGR_Fld_Create("url", OFTString);
			OGRFieldDefnH scalexField = OGR_Fld_Create("scale_x", OFTReal);
			OGRFieldDefnH scaleyField = OGR_Fld_Create("scale_y", OFTReal);
			OGRFieldDefnH scalezField = OGR_Fld_Create("scale_z", OFTReal);
			OGRFieldDefnH yallField = OGR_Fld_Create("yall", OFTReal);
			OGRFieldDefnH rollField = OGR_Fld_Create("roll", OFTReal);
			OGRFieldDefnH pitchField = OGR_Fld_Create("pitch", OFTReal);
			OGRFieldDefnH nameField = OGR_Fld_Create("name", OFTString);
			OGRFieldDefnH tableField = OGR_Fld_Create("table", OFTString);
			OGRFieldDefnH gidField = OGR_Fld_Create("gid", OFTInteger);
			OGR_L_CreateField(m_pLayer,xField,TRUE);
			OGR_L_CreateField(m_pLayer,yField,TRUE);
			OGR_L_CreateField(m_pLayer,zField,TRUE);
			OGR_L_CreateField(m_pLayer,urlField, TRUE);
			OGR_L_CreateField(m_pLayer,scalexField, TRUE);
			OGR_L_CreateField(m_pLayer,scaleyField, TRUE);
			OGR_L_CreateField(m_pLayer,scalezField, TRUE);
			OGR_L_CreateField(m_pLayer,yallField, TRUE);
			OGR_L_CreateField(m_pLayer,rollField, TRUE);
			OGR_L_CreateField(m_pLayer,pitchField, TRUE);
			OGR_L_CreateField(m_pLayer,nameField, TRUE);
			OGR_L_CreateField(m_pLayer,tableField, TRUE);
			OGR_L_CreateField(m_pLayer,gidField, TRUE);

			return true;
		}

		bool ShapeFileDatabase::create(String url,String user,String psw)
		{
			if (FileUtil::FileExist(url))
			{
				FileUtil::FileDelete(url);
			}
			return initShpDriver(url);
		}

		bool ShapeFileDatabase::writeFeature(FeatureRecord* record)
		{
			return false;
		}

		StringVector ShapeFileDatabase::enumGeometryLayers()
		{
			StringVector tables;
			osgDB::DirectoryContents contents = osgDB::getDirectoryContents(mShpPath);
			osgDB::DirectoryContents::iterator it = contents.begin();
			for(;it != contents.end();++it)
			{
				String filename = *it;
				String fullfilename = mShpPath + "/" + filename;

				if (osgDB::REGULAR_FILE != osgDB::fileType(fullfilename))
				{
					continue;
				}

				if (osgDB::getLowerCaseFileExtension(filename) == "shp")
				{
					String shpname = osgDB::getNameLessAllExtensions(filename);
					tables.push_back(shpname);
				}
			}

			return tables;
		}

		StringVector ShapeFileDatabase::emunFields(String table,String hdr)
		{
			OGRDataSourceH _dataHandle = open_shapefile(table);
			return simplequery("SELECT DISTINCT " + hdr + " FROM " + table,_dataHandle);
		}

		StringVector ShapeFileDatabase::enumNets()
		{
			StringVector tables;
			osgDB::DirectoryContents contents = osgDB::getDirectoryContents(mShpPath);
			osgDB::DirectoryContents::iterator it = contents.begin();
			for(;it != contents.end();++it)
			{
				String filename = *it;
				String fullfilename = mShpPath + "/" + filename;

				if (osgDB::REGULAR_FILE != osgDB::fileType(fullfilename))
				{
					continue;
				}

				if (osgDB::getLowerCaseFileExtension(filename) == "shp")
				{
					String shpname = osgDB::getNameLessAllExtensions(filename);
					StringVector sv = StringUtil::split(shpname, PipeTableSplice);
					if (sv.size() == 2)
					{
						String net = sv[0];
						if (sv[1] == NodeTableSubfix)
						{
							if(find(tables.begin(),tables.end(),net) == tables.end())
							{
								if(osgDB::fileExists(mShpPath + "/" + net + LineTableSubfix + ".shp"))
								{
									tables.push_back(net);
								}
							}
						}
					}
				}
			}

			return tables;
		}

		StringVector ShapeFileDatabase::simplequery(String sqlStr,OGRDataSourceH datasrc)
		{
			StringVector sv;

			if (datasrc == NULL)
			{
				return sv;
			}

			OGRLayerH _resultSetHandle = OGR_DS_ExecuteSQL( datasrc, sqlStr.c_str(), 0L, 0L );
			if ( _resultSetHandle )
			{
				OGR_L_ResetReading( _resultSetHandle );
			}
			OGRFeatureH _nextHandleToQueue = NULL;
			while(_nextHandleToQueue = OGR_L_GetNextFeature( _resultSetHandle ))
			{
				osg::ref_ptr<osgEarth::Feature> f = osgEarth::OgrUtils::createFeature( _nextHandleToQueue,(const osgEarth::FeatureProfile*)0L);
				const osgEarth::AttributeTable& attris = f->getAttrs();
				if (!attris.empty())
				{
					sv.push_back(attris.begin()->second.getString());
				}
			}
			OGR_DS_ReleaseResultSet( datasrc, _resultSetHandle );
			return sv;
		}

		StringVector ShapeFileDatabase::simplequery(String sqlStr)
		{
			StringVector sv;

			if (mSourceMap.begin() != mSourceMap.end())
			{
				OGRDataSourceH mDsHandle = mSourceMap.begin()->second;
				sv = simplequery(sqlStr,mDsHandle);
			}

			return sv;
		}

		TemplateRecordList ShapeFileDatabase::query(String table,OCHeaderInfo* headerinfo,CQueryInfo* filter)
		{
			TemplateRecordList list;

			if (headerinfo == NULL)
			{
				return list;
			}

			osg::ref_ptr<CQueryInfo> query_info = filter ? filter : new CQueryInfo;

			String _source = table;
			
			if (!FileUtil::FileExist(_source))
			{
				if (osgDB::fileType(mShpPath) == osgDB::DIRECTORY)
				{
					_source = mShpPath + "\\" + table + ".shp";
				}
			}

			_source = osgDB::convertFileNameToWindowsStyle(_source);
			StringUtil::toLowerCase(_source);

			OGRDataSourceH _dsHandle = open_shapefile(_source);

			if (_dsHandle == NULL)
			{
				return list;
			}

			//if (mSourceMap.find(_source) != mSourceMap.end())
			//{
			//	_dsHandle = mSourceMap[_source];
			//}
			//else
			//{
			//	int openMode = 0;//read only
			//	_dsHandle = OGROpen( A2U8(_source).c_str(), openMode, &_ogrDriverHandle );
			//	if (_dsHandle)
			//	{
			//		mSourceMap[_source] = _dsHandle;
			//	}
			//}

			OGRLayerH _layerHandle = NULL;
			if (_dsHandle)
			{
				_layerHandle = openLayer(_dsHandle, "");
			}

			if (_layerHandle == NULL)
			{
				return list;
			}

			OGRFeatureDefnH featureDef = OGR_L_GetLayerDefn(_layerHandle);
			String from = /*osgDB::convertStringFromUTF8toCurrentCodePage*/(OGR_FD_GetName(featureDef));

			bool hasPK = headerinfo->getPrimaryFiledInfo() != NULL;
			if (hasPK)
			{
				String pk = headerinfo->getPrimaryFiledInfo()->Name;
				StringUtil::toLowerCase(pk);
				hasPK = pk == "fid" || OGR_FD_GetFieldIndex(featureDef,pk.c_str()) > -1;
			}

			StringStream buf;
			buf << "SELECT ";

			for (int i=0;i<headerinfo->getFieldNum();i++)
			{
				OCFieldInfo* fieldinfo = headerinfo->getFieldInfo(i);

				String fn = fieldinfo->Name;
				if (fieldinfo->asGeomFieldInfo())
				{
					continue;
				}
				buf << "\"" << fn << "\",";
			}

			String expr = buf.str();

			expr[expr.length()-1] = ' ';
			expr += "from \"" + from + "\"";

			if (query_info->keyExists(CQueryInfo::NameClause))
			{
				expr += " WHERE " + query_info->getStringValue(CQueryInfo::NameClause);
			}
			
			//expr = "";
			
			//WString expr2 = L"SELECT * FROM ¹Ü¼ÜÁ¢Öù";
			//expr = A2U8(expr);
			//expr = osgDB::convertStringFromUTF8toCurrentCodePage(expr);
			OGRGeometryH _spatialFilter = NULL;
			OGRLayerH _resultSetHandle = OGR_DS_ExecuteSQL( _dsHandle, expr.c_str(), _spatialFilter, 0L );

			if ( _resultSetHandle == 0L)
			{
				return list;
			}

			OGR_L_ResetReading( _resultSetHandle );

			OGRFeatureH _nextHandleToQueue = NULL;
			while(_nextHandleToQueue = OGR_L_GetNextFeature( _resultSetHandle ))
			{
				osg::ref_ptr<osgEarth::Feature> f = osgEarth::OgrUtils::createFeature( _nextHandleToQueue,(const osgEarth::FeatureProfile*)0L);

				osg::ref_ptr<TemplateRecord> templaterecord = new TemplateRecord(headerinfo);
				const osgEarth::AttributeTable& attris = f->getAttrs();

				for (int j=0;j<headerinfo->getFieldNum();j++)
				{
					OCFieldInfo* fieldinfo = headerinfo->getFieldInfo(j);
					if (fieldinfo->asGeomFieldInfo())
					{
						osgEarth::Geometry* geom = f->getGeometry();
						if (geom == NULL)
						{
							templaterecord = NULL;
							break;
						}
						//if (geom->getType() == osgEarth::Symbology::Geometry::TYPE_POLYGON)
						//{
						//	std::string geometry = osgEarth::Features::Features::GeometryUtils::geometryToGeoJSON( geom);
						//	templaterecord->setGeometry(geometry);
						//}
						//else
						{
							Vector3List points;
							for (int i=0;i<(int)geom->size();i++)
							{
								CVector3 p = Vec3ToVector3(geom->at(i));
								if (p.x >= 1e+100 || p.x <= -1e+100
									|| p.y >= 1e+100 || p.y <= -1e+100)
								{
									continue;
								}
								points.push_back(p);
							}
							templaterecord->setGeometry(points);
						}
					}
					else
					{
						String fn = fieldinfo->Name;
						osgEarth::AttributeTable::const_iterator it = attris.find(fn);
						templaterecord->setFieldValue(fn,it->second.getString());
					}
				}

				if (templaterecord.valid())
				{
					if (!hasPK)
					{
						templaterecord->setID(OGR_F_GetFID(_nextHandleToQueue));
					}

					list.push_back(templaterecord);

					if (filter && filter->getCallback())
					{
						filter->getCallback()->onQuery(templaterecord);
					}
				}

				OGR_F_Destroy(_nextHandleToQueue);
			}        

			//release
			if ( _nextHandleToQueue )
				OGR_F_Destroy( _nextHandleToQueue );

			if ( _resultSetHandle != _layerHandle )
				OGR_DS_ReleaseResultSet( _dsHandle, _resultSetHandle );

			if ( _spatialFilter )
				OGR_G_DestroyGeometry( _spatialFilter );

			return list;
		}
		
		OGRDataSourceH ShapeFileDatabase::open_shapefile(String file)
		{
			String _source = osgDB::convertFileNameToWindowsStyle(file);
			StringUtil::toLowerCase(_source);

			OGRDataSourceH _dsHandle;

			if (mSourceMap.find(_source) != mSourceMap.end())
			{
				_dsHandle = mSourceMap[_source];
			}
			else
			{
				String fielname = _source;
				if (!FileUtil::FileExist(fielname))
				{
					fielname = mShpPath + "\\"  + _source + ".shp";
				}

				int openMode = 0;//read only

				CPLSetConfigOption("SHAPE_ENCODING", "");
				CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");

				_dsHandle = OGROpen(A2U8(fielname).c_str(), openMode, &_ogrDriverHandle);
				if (_dsHandle != NULL)
				{
					mSourceMap[_source] = _dsHandle;
				}
			}

			return _dsHandle;
		
		}

		OCHeaderInfo* ShapeFileDatabase::getHeadInfo(String table)
		{			
			String _source = osgDB::convertFileNameToWindowsStyle(table);
			StringUtil::toLowerCase(_source);

			OGRDataSourceH _dsHandle = open_shapefile(table);

			if (_dsHandle == NULL)
			{
				return NULL;
			}

			OGRLayerH _layerHandle = openLayer(_dsHandle, "");

			if (_layerHandle == NULL)
			{
				return NULL;
			}

			OGRFeatureDefnH featureDef = OGR_L_GetLayerDefn(_layerHandle);

			osg::ref_ptr<OCHeaderInfo> header = new OCHeaderInfo;
			String fidcol = OGR_L_GetFIDColumn(_layerHandle);
			if (fidcol.empty())
			{
				OCFieldInfo* fieldinfo = new OCFieldInfo("fid",OC_DATATYPE_INTEGER);
				fieldinfo->IsPrimary = true;
				header->add(fieldinfo);
			}
			int fc = OGR_FD_GetFieldCount(featureDef);
			for (int i=0;i<fc;i++)
			{
				OGRFieldDefnH fdef = OGR_FD_GetFieldDefn(featureDef,i);

				String fn = OGR_Fld_GetNameRef(fdef);
				OGRFieldType ft = OGR_Fld_GetType(fdef);
				_OC_DATATYPE dt;
				if (ft == OFTInteger)
				{
					dt = OC_DATATYPE_INTEGER;
				}
				else if (ft == OFTReal)
				{
					dt = OC_DATATYPE_FLOAT;
				}
				else if (ft == OFTDateTime)
				{
					dt = OC_DATATYPE_TIME;
				}
				else
				{
					dt = OC_DATATYPE_TEXT;
				}
				header->add(new OCFieldInfo(fn,dt));
			}

			return header.release();
		}


		inline void SqlFilter(CQueryInfo* query_info,CDataFilter* filter,OCHeaderInfo* headerinfo)
		{
			if (filter == NULL)
			{
				return;
			}

			String clause = filter->getNodeWhereClause();

			longVector ids = filter->getNodeIDList();
			if (!ids.empty())
			{
				String idsClause;
				long n = (long)ids.size();
				for (long i=0;i<n;i++)
				{
					idsClause += StringConverter::toString(ids[i]);
					if (i != n - 1)
					{
						idsClause += ",";
					}
				}

				String idclause = headerinfo->getPrimaryFiledInfo()->Name + " in (" + idsClause + ")";

				if (!clause.empty())
				{
					clause += " AND (" + idclause + ")";
				}
				else
				{
					clause = idclause;
				}
			}

			if (!clause.empty())
			{
				query_info->setConfig(CQueryInfo::NameClause,clause);
			}
		}

		bool ShapeFileDatabase::getLineData(PipeTable* table)
		{			
			OCHeaderInfo* headerinfo = table->getQueryMapping()->getLineQueryHeader();

			if (headerinfo == NULL)
			{
				return false;
			}

			osg::ref_ptr<CQueryInfo> query_info = new CQueryInfo;
			SqlFilter(query_info,table->getLineTable().getFilter(),headerinfo);
			query_info->setCallback(new ModelDataQueryCallback(table,ModelDataQueryCallback::Type_Line));

			TemplateRecordList list = query(table->getName() + LineTableSubfix,headerinfo,query_info);
			return !list.empty();
		}

		bool ShapeFileDatabase::getNodeData(PipeTable* table)
		{			
			OCHeaderInfo* headerinfo = table->getQueryMapping()->getNodeQueryHeader();

			if (headerinfo == NULL)
			{
				return false;
			}

			osg::ref_ptr<CQueryInfo> query_info = new CQueryInfo;
			SqlFilter(query_info,table->getNodeTable().getFilter(),headerinfo);
			query_info->setCallback(new ModelDataQueryCallback(table,ModelDataQueryCallback::Type_Node));

			TemplateRecordList list = query(table->getName() + NodeTableSubfix,headerinfo,query_info);
			return !list.empty();
		}
	}	
}