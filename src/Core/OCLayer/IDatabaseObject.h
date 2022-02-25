#ifndef _OC_DATABASEOBJECT_H__
#define _OC_DATABASEOBJECT_H__

#include "LayerDefine.h"
#include "OCMain/DataStream.h"

namespace OC
{
	class _LayerExport OCFieldInfo : public osg::Referenced
	{
	public:
		OCFieldInfo();
		OCFieldInfo(String _name,OC_DATATYPE _type,bool _isprimary = false);
		virtual OCGeomFieldInfo* asGeomFieldInfo(){return NULL;}
		virtual OCJoinFieldInfo* asJoinFieldInfo() { return NULL; }
		virtual OCBinaryFieldInfo* asBinaryFieldInfo() { return NULL; }

		int Scale;
		int Length;
		int Precision;
		bool IsPrimary;
		String Name;
		String AliasName;
		OC_DATATYPE TYPE;
		static OCFieldInfo* fromXmlElement(TiXmlElement* element);
		virtual TiXmlElement* toXmlElement();
	protected:
		~OCFieldInfo();
	};

	class _LayerExport OCJoinFieldInfo : public OCFieldInfo
	{
	public:
		OCJoinFieldInfo(String _name, OC_DATATYPE _type);
		~OCJoinFieldInfo(){}
		virtual OCJoinFieldInfo* asJoinFieldInfo(){return static_cast<OCJoinFieldInfo*>(this);}
		virtual TiXmlElement* toXmlElement();

		void setJoinClause(String clause){mJoinClause = clause;}
		String getJoinClause(){return mJoinClause;}

		void setJoinTable(String table){mJoinTable = table;}
		String getJoinTable(){return mJoinTable;}

	private:
		String mJoinTable;
		String mJoinClause;
	};

	class _LayerExport OCGeomFieldInfo : public OCFieldInfo
	{
	public:
		OCGeomFieldInfo(String _name, String _geometryType, int _srid);
		virtual OCGeomFieldInfo* asGeomFieldInfo();
		int SRID;
		String GeometryType;
		virtual TiXmlElement* toXmlElement();

		void setRelatedX(String fieldinfo);
		void setRelatedY(String fieldinfo);

		String getRelatedX();
		String getRelatedY();

		static Vector3List parseSpatialGeom(String geom);
		static void trim(String& geomStr);
		static String TYPE_POINT;
		static String TYPE_LINESTRING;
		static String TYPE_POLYGON;
	protected:
		String relatedX;
		String relatedY;
		~OCGeomFieldInfo();
	};

	class _LayerExport OCBinaryFieldInfo : public OCFieldInfo
	{
	public:
		OCBinaryFieldInfo(String _name);
		virtual OCBinaryFieldInfo* asBinaryFieldInfo() { return static_cast<OCBinaryFieldInfo*>(this); }
	protected:
		~OCBinaryFieldInfo() {}
	};

	class _LayerExport OCSpatialTable : public osg::Referenced
	{
	public:
		OCSpatialTable(){}
		~OCSpatialTable(){}
		
		void setGeomFieldInfo(OCGeomFieldInfo* geomfield){mGeomFieldInfo = geomfield;}
		OCGeomFieldInfo* getGeomFieldInfo(){return mGeomFieldInfo.get();}

		void setName(String name){mTableName = name;}
		String getName(){return mTableName;}
	private:
		osg::ref_ptr<OCGeomFieldInfo> mGeomFieldInfo;
		String mTableName;
	};

	typedef std::vector<osg::ref_ptr<OCFieldInfo> > FieldInfoList;
	typedef std::map<String,osg::ref_ptr<OCFieldInfo> > FieldInfoMap;

	typedef ConstVectorIterator<FieldInfoList> FieldInfoListIterator;
	typedef MapIterator<FieldInfoMap> FieldInfoMaptIterator;

	class _LayerExport OCHeaderInfo : public osg::Referenced
	{
	public:
		OCHeaderInfo();
		OCHeaderInfo(String primarykey);
		~OCHeaderInfo();

		void clear();
		void add(OCFieldInfo* fieldinfo);
		bool exist(String fieldname);
		int getFieldNum();
		void replaceFieldInfo(int i,OCFieldInfo* fieldinfo);
		void replaceFieldInfo(String fieldname,OCFieldInfo* fieldinfo);
		OCFieldInfo* getFieldInfo(int i);
		OCFieldInfo* getFieldInfo(String fieldname);
		OCFieldInfo* getPrimaryFiledInfo();
		OCGeomFieldInfo* getGeomFieldInfo();
		int getJoinFieldCount() { return (int)mJoinFieldInfo.size(); }
		OCJoinFieldInfo* getJoinFieldInfo(int i);
		StringMap& getFieldMapping(){return mFieldMap;}
		void fromXmlElement(TiXmlElement* element);
		TiXmlElement* toXmlElement(TiXmlElement* bindElement = NULL);
	private:
		FieldInfoList							mFieldInfoList;
		FieldInfoMap							mFieldInfoMap;
		OCFieldInfo*							mPrimaryFieldInfo;
		OCGeomFieldInfo*						mGeomFieldInfo;
		std::vector<OCJoinFieldInfo*>			mJoinFieldInfo;
		StringMap								mFieldMap;
	};

	class OCField
	{
	public:
		OCField() {}
		virtual ~OCField() {}
		virtual OCBinaryField* asBinaryField() { return NULL; }
		String					  NAME;
		String					  VALUE;
	};

	class OCBinaryField : public OCField
	{
	public:
		String					  NAME;
		MemoryDataStreamPtr		  DataStream;
		virtual OCBinaryField* asBinaryField() { return static_cast<OCBinaryField*>(this); }
	};

	typedef std::map<String, OCField*> FieldMap;

	class _LayerExport TemplateRecord : public osg::Referenced
	{
	public:
		TemplateRecord();
		TemplateRecord(OCHeaderInfo* headinfo);
		~TemplateRecord();
		void setGeometry(String val);
		void setGeometry(CVector2 position);
		void setGeometry(Vector2List linepoints);
		void setGeometry(Vector3List linepoints);


		void setFieldValue(String fieldname,String val);
		void setFieldValue(String fieldname,int val);
		void setFieldValue(String fieldname,long val);
		void setFieldValue(String fieldname,bool val);
		void setFieldValue(String fieldname, double val);
		void setFieldValue(String fieldname, MemoryDataStreamPtr val);

		void reset();

		void setID(long id);
		long getID();

		void setPrimaryValue(String val);
		String getPrimaryValue();

		Vector3List parseGeometry();
		String getGeometry();
		String getFieldValue(String fieldname);

		int getIntValue(String fieldname,int defaultValue = -1);
		long getLongValue(String fieldname,long defaultValue = -1);
		bool getBoolValue(String fieldname,bool defaultValue = false);
		double getDoubleValue(String fieldname, double defaultValue = 0.0f);
		CVector3 getVector3(String fn1, String fn2, String fn3, CVector3 defaultValue = CVector3::ZERO);
		MemoryDataStreamPtr getBlobValue(String fieldname);

		void setHeadInfo(OCHeaderInfo* headinfo);
		OCHeaderInfo* getHeadInfo();
		OCHeaderInfo* getOrHeadInfo();

	protected:
		FieldMap								mFields;
		osg::ref_ptr<OCHeaderInfo>				mHeadInfo;
	};

	typedef std::vector<osg::ref_ptr<TemplateRecord> > TemplateRecordList;
}

#endif
