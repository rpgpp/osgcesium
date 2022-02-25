#include "IDatabaseObject.h"

namespace OC
{
	OCFieldInfo::OCFieldInfo()
		:TYPE(OC_DATATYPE_TEXT)
		,Length(0)
		,Precision(0)
		,Scale(0)
		,IsPrimary(false)
	{

	}

	OCFieldInfo::OCFieldInfo(String _name,OC_DATATYPE _type,bool _isprimary)
		:Name(_name)
		,TYPE(_type)
		,Length(0)
		,Precision(0)
		,Scale(0)
		,IsPrimary(_isprimary)
	{

	}

	OCFieldInfo::~OCFieldInfo()
	{

	}

	OCFieldInfo* OCFieldInfo::fromXmlElement(TiXmlElement* element)
	{
		OCFieldInfo* fieldinfo = NULL;
		if (const char * t = element->GetText())
		{
			String ft = TinyXml::read_attribute(element,"type");
			StringUtil::toLowerCase(ft);

			OC_DATATYPE type;

			if (ft == "int")
			{
				type = OC_DATATYPE_INTEGER;
			}
			else if (ft == "blob")
			{
				type = OC_DATATYPE_BLOB;
			}
			else if (ft == "float")
			{
				type = OC_DATATYPE_FLOAT;
			}
			else if (ft == "time")
			{
				type = OC_DATATYPE_TIME;
			}
			else if (ft == "geometry")
			{
				type = OC_DATATYPE_GEOM;
			}
			else
			{
				type = OC_DATATYPE_TEXT;
			}

			String fn(t);

			bool primaryKey = StringConverter::parseBool(TinyXml::read_attribute(element,"PrimaryKey"));

			if (type == OC_DATATYPE_GEOM)
			{
				int srid = StringConverter::parseInt(TinyXml::read_attribute(element,"SRID"));
				String gtype = TinyXml::read_attribute(element,"GeometryType");
				fieldinfo = new OCGeomFieldInfo(fn,gtype,srid);
				((OCGeomFieldInfo*)fieldinfo)->setRelatedX(TinyXml::read_attribute(element,"relatedX"));
				((OCGeomFieldInfo*)fieldinfo)->setRelatedY(TinyXml::read_attribute(element,"relatedY"));
			}
			else
			{
				String joinTable = TinyXml::read_attribute(element,"JoinTable");
				String joinClause = TinyXml::read_attribute(element,"JoinClause");
				if (joinTable.empty())
				{
					fieldinfo = new OCFieldInfo(fn, type);
				}
				else
				{
					fieldinfo = new OCJoinFieldInfo(fn, type);
					((OCJoinFieldInfo*)fieldinfo)->setJoinTable(joinTable);
					((OCJoinFieldInfo*)fieldinfo)->setJoinClause(joinClause);
				}
				fieldinfo->IsPrimary = primaryKey;
			}
		}

		return fieldinfo;
	}

	TiXmlElement* OCFieldInfo::toXmlElement()
	{
		TiXmlElement* field = new TiXmlElement("Field");
		String ft;
		switch(TYPE)
		{
		case OC_DATATYPE_INTEGER:
			ft = "int";
			break;
		case OC_DATATYPE_FLOAT:
			ft = "float";
			break;
		case OC_DATATYPE_TIME:
			ft = "time";
			break;
		case OC_DATATYPE_GEOM:
			ft = "geometry";
			break;
		case OC_DATATYPE_BLOB:
			ft = "blob";
			break;
		default:
			ft = "text";
			break;
		}
		field->SetAttribute("type",ft);
		field->LinkEndChild(new TiXmlText(Name));
		if (IsPrimary)
		{
			field->SetAttribute("PrimaryKey",StringConverter::toString(IsPrimary));
		}
		return field;
	}

	//////////////////////////////////////////////////////////////////////////
	OCJoinFieldInfo::OCJoinFieldInfo(String _name, OC_DATATYPE _type)
		:OCFieldInfo(_name, _type)
	{

	}


	//////////////////////////////////////////////////////////////////////////
	OCGeomFieldInfo::OCGeomFieldInfo(String _name,String _geometryType,int _srid)
		:OCFieldInfo(_name,OC_DATATYPE_GEOM)
		,SRID(_srid)
		,GeometryType(_geometryType)
	{

	}

	OCGeomFieldInfo::~OCGeomFieldInfo()
	{
	
	}

	OCGeomFieldInfo* OCGeomFieldInfo::asGeomFieldInfo()
	{
		return static_cast<OCGeomFieldInfo*>(this);
	}

	Vector3List OCGeomFieldInfo::parseSpatialGeom(String geomStr)
	{
		Vector3List lineList;

		//LINESTRING(552006.765 3390161.683,551931.414 3390124.603)
		//LINESTRING (513813.602 389905.845, 513813.585 389896.883)
		//LINESTRING Z (551493.8448896 3388362.0965968 28.8,551492.6185414 3388365.7724226 31)'

		size_t pos1 = geomStr.find("(");
		size_t pos2 = geomStr.find_last_not_of(")");

		String lineStr = geomStr;

		if (pos1 != String::npos && pos2 != String::npos)
		{
			lineStr = geomStr.substr(pos1 + 1,pos2 - pos1);
			StringUtil::trim(lineStr,true,true,"()");
		}
		else
		{
			StringUtil::trim(lineStr);
		}

		StringVector pointVector = StringUtil::split(lineStr,",");

		StringVector::iterator it = pointVector.begin();
		for (;it!=pointVector.end();it++)
		{
			String pointString = *it;
			StringUtil::trim(pointString);

			StringVector sv = StringUtil::split(pointString);
			if (sv.size() == 2)
			{
				CVector3 p = CVector3::ZERO;
				p.x = StringConverter::parseReal(sv[0]);
				p.y = StringConverter::parseReal(sv[1]);
				lineList.push_back(p);
			}
			else if (sv.size() > 2)
			{
				CVector3 p = CVector3::ZERO;
				p.x = StringConverter::parseReal(sv[0]);
				p.y = StringConverter::parseReal(sv[1]);
				p.z = StringConverter::parseReal(sv[2]);
				lineList.push_back(p);
			}
		}

		return lineList;
	}

	TiXmlElement* OCGeomFieldInfo::toXmlElement()
	{
		TiXmlElement* element = OCFieldInfo::toXmlElement();
		element->SetAttribute("SRID",StringConverter::toString(SRID));
		element->SetAttribute("GeometryType",GeometryType);
		if (!relatedX.empty())
		{
			element->SetAttribute("relatedX",relatedX);
		}
		if (!relatedY.empty())
		{
			element->SetAttribute("relatedY",relatedY);
		}
		return element;
	}

	void OCGeomFieldInfo::trim(String& geomStr)
	{
		StringUtil::trim(geomStr,true,true," \r\tPOINTLINESTRING()MUZ");
	}

	void OCGeomFieldInfo::setRelatedX(String fieldinfo)
	{
		relatedX = fieldinfo;
	}

	void OCGeomFieldInfo::setRelatedY(String fieldinfo)
	{
		relatedY = fieldinfo;
	}

	String OCGeomFieldInfo::getRelatedX()
	{
		return relatedX;
	}

	String OCGeomFieldInfo::getRelatedY()
	{
		return relatedY;
	}

	String OCGeomFieldInfo::TYPE_POINT = "POINT";
	String OCGeomFieldInfo::TYPE_LINESTRING = "LINESTRING";
	String OCGeomFieldInfo::TYPE_POLYGON = "POLYGON";

	//////////////////////////////////////////////////////////////////////////
	TiXmlElement* OCJoinFieldInfo::toXmlElement()
	{
		TiXmlElement* element = OCFieldInfo::toXmlElement();
		element->SetAttribute("JoinTable",mJoinTable);
		element->SetAttribute("JoinClause",mJoinClause);
		return element;
	}

	//////////////////////////////////////////////////////////////////////////
	OCBinaryFieldInfo::OCBinaryFieldInfo(String _name)
		:OCFieldInfo(_name,OC_DATATYPE_BLOB)
	{

	}


	//////////////////////////////////////////////////////////////////////////
	OCHeaderInfo::OCHeaderInfo()
		:mGeomFieldInfo(NULL)
		,mPrimaryFieldInfo(NULL)
	{

	}

	OCHeaderInfo::OCHeaderInfo(String primarykey)
		:mGeomFieldInfo(NULL)
		,mPrimaryFieldInfo(NULL)
	{
		add(new OCFieldInfo(primarykey,OC_DATATYPE_INTEGER,true));
	}

	OCHeaderInfo::~OCHeaderInfo()
	{

	}

	void OCHeaderInfo::fromXmlElement(TiXmlElement* element)
	{
		if ( element == NULL)
		{
			return;
		}

		mFieldInfoList.clear();
		mFieldInfoMap.clear();
		mGeomFieldInfo = NULL;
		mPrimaryFieldInfo = NULL;

		TiXmlElement* field = element->FirstChildElement("Field");
		for (;field;field = field->NextSiblingElement("Field"))
		{
			osg::ref_ptr<OCFieldInfo> fieldinfo  = OCFieldInfo::fromXmlElement(field);

			String fn = fieldinfo->Name;
			if (getFieldInfo(fn) == NULL)
			{
				add(fieldinfo);
			}

			//alias name
			String name = TinyXml::read_attribute(field,"name");
			if (!name.empty())
			{
				mFieldMap[name] = fn;
			}
		}
	}

	TiXmlElement* OCHeaderInfo::toXmlElement(TiXmlElement* bindElement)
	{
		TiXmlElement* element = (bindElement ==  NULL ? new TiXmlElement("OCHeaderInfo") : bindElement);

		if (!mFieldMap.empty())
		{
			StringMap::iterator it = mFieldMap.begin();
			for(;it!=mFieldMap.end();it++)
			{
				if (OCFieldInfo* fieldinfo = getFieldInfo(it->second))
				{
					TiXmlElement* fieldelement = fieldinfo->toXmlElement();
					fieldelement->SetAttribute("name",it->first);
					element->LinkEndChild(fieldelement);
				}
			}
		}
		else
		{
			for (int i=0;i<getFieldNum();i++)
			{
				OCFieldInfo* fieldinfo = getFieldInfo(i);
				element->LinkEndChild(fieldinfo->toXmlElement());
			}
		}

		return element;
	}

	OCFieldInfo* OCHeaderInfo::getPrimaryFiledInfo()
	{
		return mPrimaryFieldInfo;
	}

	OCJoinFieldInfo* OCHeaderInfo::getJoinFieldInfo(int i)
	{
		return mJoinFieldInfo[i];
	}
	
	OCGeomFieldInfo* OCHeaderInfo::getGeomFieldInfo()
	{
		return mGeomFieldInfo;
	}

	void OCHeaderInfo::clear()
	{
		mFieldInfoList.clear();
		mFieldInfoMap.clear();
		mFieldMap.clear();
		mPrimaryFieldInfo = NULL;
		mGeomFieldInfo = NULL;
	}

	void OCHeaderInfo::add(OCFieldInfo* fieldinfo)
	{
		if (fieldinfo->TYPE == OC_DATATYPE_GEOM)
		{
			mGeomFieldInfo = fieldinfo->asGeomFieldInfo();
		}

		if (fieldinfo->asJoinFieldInfo())
		{
			mJoinFieldInfo.push_back(fieldinfo->asJoinFieldInfo());
		}

		if (fieldinfo->IsPrimary)
		{
			mPrimaryFieldInfo = fieldinfo;
		}

		mFieldInfoList.push_back(fieldinfo);
		mFieldInfoMap[fieldinfo->Name] = fieldinfo;
	}

	bool OCHeaderInfo::exist(String fieldname)
	{
		return mFieldInfoMap.find(fieldname) != mFieldInfoMap.end() || mFieldMap.find(fieldname) != mFieldMap.end();
	}

	int OCHeaderInfo::getFieldNum()
	{
		return (int)mFieldInfoList.size();
	}

	void OCHeaderInfo::replaceFieldInfo(String fieldname,OCFieldInfo* fieldinfo)
	{
		for(int i=0;i<getFieldNum();i++)
		{
			if (getFieldInfo(i)->Name == fieldname)
			{
				replaceFieldInfo(i,fieldinfo);
				break;
			}
		}
	}

	void OCHeaderInfo::replaceFieldInfo(int i,OCFieldInfo* fieldinfo)
	{
		osg::ref_ptr<OCFieldInfo> old = getFieldInfo(i);
		if (old.valid())
		{
			mFieldInfoList[i] = fieldinfo;
			mFieldInfoMap.erase(mFieldInfoMap.find(old->Name));
			mFieldInfoMap[fieldinfo->Name] = fieldinfo;
			if (old->IsPrimary)
			{
				mPrimaryFieldInfo = fieldinfo;
			}
			
			if (fieldinfo->asGeomFieldInfo())
			{
				mGeomFieldInfo = fieldinfo->asGeomFieldInfo();
			}

			if (OCJoinFieldInfo* joinfield = fieldinfo->asJoinFieldInfo())
			{
				if(old->asJoinFieldInfo())
				{
					mJoinFieldInfo.clear();
					for (int j=0;j<(int)mFieldInfoList.size();j++)
					{
						if (mFieldInfoList[j]->asJoinFieldInfo())
						{
							mJoinFieldInfo.push_back(mFieldInfoList[j]->asJoinFieldInfo());
						}
					}
				}
				else
				{
					mJoinFieldInfo.push_back(joinfield);
				}
			}

			StringMap::iterator it = mFieldMap.begin();
			for (;it!=mFieldMap.end();it++)
			{
				if (it->second == old->Name)
				{
					it->second = fieldinfo->Name;
					break;
				}
			}
		}
	}

	OCFieldInfo* OCHeaderInfo::getFieldInfo(int i)
	{
		if (i>-1 && i<getFieldNum())
		{
			return mFieldInfoList[i].get();
		}
		else
		{
			return NULL;
		}
	}

	OCFieldInfo* OCHeaderInfo::getFieldInfo(String fieldname)
	{
		FieldInfoMap::iterator it = mFieldInfoMap.find(fieldname);

		if (it!=mFieldInfoMap.end())
		{
			return it->second.get();
		}
		else
		{
			return NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	TemplateRecord::TemplateRecord()
	{
	}

	TemplateRecord::TemplateRecord(OCHeaderInfo* headinfo)
	{
		setHeadInfo(headinfo);
	}

	TemplateRecord::~TemplateRecord()
	{
		CORE_DELETE_SECOND(mFields, FieldMap);
	}

	void TemplateRecord::setHeadInfo(OCHeaderInfo*headinfo)
	{
		mHeadInfo = headinfo;
	}

	OCHeaderInfo* TemplateRecord::getHeadInfo()
	{
		return mHeadInfo.get();
	}

	OCHeaderInfo* TemplateRecord::getOrHeadInfo()
	{
		if (!mHeadInfo.valid())
		{
			mHeadInfo = new OCHeaderInfo;
		}
		return mHeadInfo.get();
	}

	void TemplateRecord::setPrimaryValue(String val)
	{
		if (OCFieldInfo* field = mHeadInfo->getPrimaryFiledInfo())
		{
			return setFieldValue(field->Name,val);
		}
	}

	String TemplateRecord::getPrimaryValue()
	{
		if (OCFieldInfo* field = mHeadInfo->getPrimaryFiledInfo())
		{
			return getFieldValue(field->Name);
		}
		return StringUtil::BLANK;
	}

	void TemplateRecord::setID(long id)
	{
		setPrimaryValue(StringConverter::toString(id));
	}

	long TemplateRecord::getID()
	{
		return StringConverter::parseLong(getPrimaryValue(),-1);
	}

	Vector3List TemplateRecord::parseGeometry()
	{
		String geom  = getGeometry();
		return OCGeomFieldInfo::parseSpatialGeom(geom);
	}

	String TemplateRecord::getGeometry()
	{
		if (OCGeomFieldInfo* field = mHeadInfo->getGeomFieldInfo())
		{
			return getFieldValue(field->Name);
		}

		return StringUtil::BLANK;
	}

	int TemplateRecord::getIntValue(String fieldname,int defaultValue)
	{
		return StringConverter::parseInt(getFieldValue(fieldname),defaultValue);
	}

	bool TemplateRecord::getBoolValue(String fieldname,bool defaultValue)
	{
		return StringConverter::parseBool(getFieldValue(fieldname),defaultValue);
	}

	long TemplateRecord::getLongValue(String fieldname,long defaultValue)
	{
		return StringConverter::parseLong(getFieldValue(fieldname),defaultValue);
	}

	double TemplateRecord::getDoubleValue(String fieldname,double defaultValue)
	{
		return StringConverter::parseReal(getFieldValue(fieldname),defaultValue);
	}

	CVector3 TemplateRecord::getVector3(String fn1, String fn2, String fn3, CVector3 defaultValue)
	{
		String x = getFieldValue(fn1);
		String y = getFieldValue(fn2);
		String z = getFieldValue(fn3);
		return StringConverter::parseVector3(x + " " + y + " " + z,defaultValue);
	}

	MemoryDataStreamPtr TemplateRecord::getBlobValue(String fieldname)
	{
		MemoryDataStreamPtr data;

		FieldMap::iterator it = mFields.find(fieldname);

		if (it == mFields.end())
		{
			it = mFields.find(mHeadInfo->getFieldMapping()[fieldname]);
		}

		if (it != mFields.end())
		{
			if (OCBinaryField* field = it->second->asBinaryField())
			{
				data = field->DataStream;
			}
		}

		return data;
	}


	String TemplateRecord::getFieldValue(String fieldname)
	{
		FieldMap::iterator it = mFields.find(fieldname);

		if (it== mFields.end())
		{
			it = mFields.find(mHeadInfo->getFieldMapping()[fieldname]);
		}

		if (it !=mFields.end())
		{
			if (OCBinaryField* field = it->second->asBinaryField())
			{
				if (!field->DataStream.isNull())
				{
					StringStream ss;
					ss.write((const char*)field->DataStream->getPtr(), field->DataStream->size());
					return ss.str();
				}
			}
			else
			{
				return it->second->VALUE;
			}
		}

		return StringUtil::BLANK;
	}

	void TemplateRecord::setGeometry(String val)
	{
		if (OCFieldInfo* field = mHeadInfo->getGeomFieldInfo())
		{
			setFieldValue(field->Name,val);
		}
	}

	void TemplateRecord::setGeometry(CVector2 position)
	{
		if (OCFieldInfo* field = mHeadInfo->getGeomFieldInfo())
		{
			String geom = "POINT (";
			geom += StringConverter::formatDoubletoString(position.x,6);
			geom += " ";
			geom += StringConverter::formatDoubletoString(position.y,6);
			geom += ")";
			setFieldValue(field->Name,geom);
		}
	}

	void TemplateRecord::setGeometry(Vector2List linepoints)
	{
		int n = (int)linepoints.size();

		if (n < 1)
		{
			return;
		}
		else if (n == 1)
		{
			setGeometry(CVector2(linepoints[0].x,linepoints[0].y));
		}
		else
		{
			if (n==2 && linepoints[0] == linepoints[1])
			{
				linepoints[1] += CVector2(1e-6,1e-6);
			}

			if (OCFieldInfo* field = mHeadInfo->getGeomFieldInfo())
			{
				String geom = "LINESTRING (";

				for (int i=0;i<n;i++)
				{
					CVector2 point = linepoints[i];
					geom += StringConverter::formatDoubletoString(point.x,6);
					geom += " ";
					geom += StringConverter::formatDoubletoString(point.y,6);
					geom += ",";
				}

				geom[geom.length()-1] = ')';

				setFieldValue(field->Name,geom);
			}
		}
	}

	void TemplateRecord::setGeometry(Vector3List linepoints)
	{
		Vector2List v2;
		v2.resize(linepoints.size());
		for (Vector2List::size_type i=0;i<linepoints.size();i++)
		{
			v2[i].x = linepoints[i].x;
			v2[i].y = linepoints[i].y;
		}
		setGeometry(v2);
	}

	void TemplateRecord::setFieldValue(String fieldname,int val)
	{
		setFieldValue(fieldname,StringConverter::toString(val));
	}

	void TemplateRecord::setFieldValue(String fieldname,long val)
	{
		setFieldValue(fieldname,StringConverter::toString(val));
	}

	void TemplateRecord::setFieldValue(String fieldname,bool val)
	{
		setFieldValue(fieldname,StringConverter::toString(val));
	}

	void TemplateRecord::setFieldValue(String fieldname,double val)
	{
		setFieldValue(fieldname,StringConverter::toString(val,10));
	}

	void TemplateRecord::setFieldValue(String fieldname, MemoryDataStreamPtr val)
	{
		if (fieldname.empty() || val.isNull())
		{
			return;
		}

		OCBinaryField* field = new OCBinaryField;
		field->NAME = fieldname;
		field->DataStream = val;

		mFields[fieldname] = field;
	}


	void TemplateRecord::setFieldValue(String fieldname,String val)
	{
		if (fieldname.empty())
		{
			return;
		}

		OCField* field = new OCField;
		field->NAME = fieldname;
		field->VALUE = val;
		mFields[fieldname] = field;
	}

	void TemplateRecord::reset()
	{
		FieldMap::iterator it = mFields.begin();
		for (;it!=mFields.end();it++)
		{
			it->second->VALUE = StringUtil::BLANK;
		}
	}
	
}
