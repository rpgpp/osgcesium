#include "IDatabase.h"
#include "FeatureRecord.h"
#include "Feature.h"
#include "FeatureNode.h"

namespace OC
{
	String FeatureRecord::makeCode(String gid,String table)
	{
		if (gid.empty() || table.empty())
		{
			return StringUtil::BLANK;
		}

		String code =  gid + "_" + table;
		StringUtil::toLowerCase(code);
		return code;
	}
	String FeatureRecord::makeCode(long gid,String table)
	{
		return makeCode(StringConverter::toString(gid),table);
	}

	FeatureRecord::FeatureRecord()
		:mScale(CVector3::UNIT_SCALE)
		,mOffset(CVector3::ZERO)
		,mGID(-1)
		,mDNO(-1)
		,mTileID(-1)
		,mModelID(-1)
		,mDatabase(NULL)
		,mIsTile(false)
	{

	}

	FeatureRecord::~FeatureRecord()
	{

	}

	String FeatureRecord::getCode()
	{
		if(mGID > 0 && !mTable.empty())
		{
			String codeStr = StringConverter::toString(mGID) + "_" + mTable;
			StringUtil::toLowerCase(codeStr);
			return codeStr;
		}

		return StringUtil::BLANK;
	}

	bool FeatureRecord::isTileModel()
	{
		return false;
	}

	void FeatureRecord::setTileModel(bool isTile)
	{
		mIsTile = isTile;
	}

	String FeatureRecord::getTable()
	{
		return mTable;
	}

	void FeatureRecord::setTable(String table)
	{
		mTable = table;
	}

	String FeatureRecord::getTime()
	{
		return mTimeStamp;
	}

	void FeatureRecord::setTime(String timeStamp)
	{
		mTimeStamp = timeStamp;
	}

	CSphere FeatureRecord::getBoundSphere()
	{
		CSphere sphere;
		CVector4 v = StringConverter::parseVector4(mBoundString,CVector4(0.0,0.0,0.0,3.0));
		sphere.setCenter(CVector3(v[0],v[1],v[2]));
		sphere.setRadius(v[3]);
		return sphere;
	}

	String FeatureRecord::getBoundString()
	{
		return mBoundString;
	}

	void FeatureRecord::setBoundString(String name)
	{
		mBoundString = name;
	}

	String FeatureRecord::getRotateString()
	{
		return mRotateString;
	}

	void FeatureRecord::setRotateString(String name)
	{
		mRotateString = name;
	}

	String FeatureRecord::getMaterialString()
	{
		return m_strMaterialString;
	}

	void FeatureRecord::setMaterialString(String name)
	{
		m_strMaterialString = name;
	}

	String FeatureRecord::getFeatureUrl()
	{
		return mFeatureUrl;
	}

	void FeatureRecord::setFeatureUrl(String name)
	{
		mFeatureUrl = name;
	}

	CVector3 FeatureRecord::getOffset()
	{
		return mOffset;
	}

	void FeatureRecord::setOffset(CVector3 offset)
	{
		mOffset = offset;
	}

	CVector3 FeatureRecord::getScale()
	{
		return mScale;
	}

	void FeatureRecord::setScale(CVector3 scale)
	{
		mScale = scale;
	}

	void FeatureRecord::setGID(int gid)
	{
		mGID  = gid;
	}

	long FeatureRecord::getGID()
	{
		return mGID;
	}

	void FeatureRecord::setDNO(int dno)
	{
		mDNO  = dno;
	}

	int FeatureRecord::getDNO()
	{
		return mDNO;
	}

	void FeatureRecord::setMID(long mid)
	{
		mModelID  = mid;
	}

	long FeatureRecord::getMID()
	{
		return mModelID;
	}

	void FeatureRecord::setTileID(int tid)
	{
		mTileID = tid;
	}

	int FeatureRecord::getTileID()
	{
		return mTileID;
	}

	IDatabase* FeatureRecord::getDatabase()
	{
		return mDatabase.get();
	}

	void FeatureRecord::setDatabase(IDatabase* database)
	{
		mDatabase = database;
	}

	MemoryDataStreamPtr& FeatureRecord::getData()
	{
		return mDataStream;
	}

	osg::Node* FeatureRecord::readNode(const osgDB::Options* options)
	{
		String ext = osgDB::getLowerCaseFileExtension(getFeatureUrl());

		if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(ext))
		{
			bool valid = false;

			StringStream sstream;
			if (validData())
			{
				sstream.write((char*)mDataStream->getPtr(),mDataStream->size());
				mDataStream.setNull();
				valid = true;
			}
			else
			{
				osg::ref_ptr<IDatabase> database;
				if (mDatabase.lock(database))
				{
					valid = database->getModelStream(getFeatureUrl(),sstream);
				}
			}

			if (valid)
			{
				osg::ref_ptr<osg::Node> node = rw->readNode(sstream,options).takeNode();
				if (node.valid())
				{
					setBoundString(parseBoundString(node.get()));
					return node.release();
				}
			}
		}
		
		return NULL;
	}

	TemplateRecord* FeatureRecord::toTemplateRecord(OCHeaderInfo* hd)
	{
		osg::ref_ptr<OCHeaderInfo> header = hd;
		if (!header.valid())
		{
			header = new OCHeaderInfo("ID");
			header->add(new OCFieldInfo("GID", OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo("DNO", OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo("NAME", OC_DATATYPE_TEXT));
			header->add(new OCFieldInfo("LAYERNAME", OC_DATATYPE_TEXT));
			header->add(new OCFieldInfo("MODELID", OC_DATATYPE_INTEGER));
			header->add(new OCFieldInfo("OFFSET_X", OC_DATATYPE_FLOAT));
			header->add(new OCFieldInfo("OFFSET_Y", OC_DATATYPE_FLOAT));
			header->add(new OCFieldInfo("OFFSET_Z", OC_DATATYPE_FLOAT));
			header->add(new OCFieldInfo("SCALE_X", OC_DATATYPE_FLOAT));
			header->add(new OCFieldInfo("SCALE_Y", OC_DATATYPE_FLOAT));
			header->add(new OCFieldInfo("SCALE_Z", OC_DATATYPE_FLOAT));
			header->add(new OCFieldInfo("ROTATE", OC_DATATYPE_TEXT));
			header->add(new OCFieldInfo("TIMESTAMP", OC_DATATYPE_TEXT));
		}
		TemplateRecord* record = new TemplateRecord(header);
		record->setFieldValue("ID", getID());
		record->setFieldValue("GID", getGID());
		record->setFieldValue("DNO", getDNO());
		record->setFieldValue("NAME", getFeatureName());
		record->setFieldValue("LAYERNAME", getTable());
		record->setFieldValue("MODELID", getMID());
		record->setFieldValue("OFFSET_X", mOffset.x);
		record->setFieldValue("OFFSET_Y", mOffset.y);
		record->setFieldValue("OFFSET_Z", mOffset.z); 
		record->setFieldValue("SCALE_X", mScale.x);
		record->setFieldValue("SCALE_Y", mScale.y);
		record->setFieldValue("SCALE_Z", mScale.z);
		record->setFieldValue("ROTATE", getRotateString());
		record->setFieldValue("TIMESTAMP", getTime());
		return record;
	}

#define SET_BOOL(func,record,name)if(header && header->exist(name))func(record->getBoolValue(name))
#define SET_INT(func,record,name)if(header && header->exist(name))func(record->getIntValue(name))
#define SET_DOUBLE(func,record,name)if(header && header->exist(name))func(record->getDoubleValue(name))
#define SET_STRING(func,record,name)if(header && header->exist(name))func(record->getFieldValue(name))
#define SET_VECTOR3(func,record,n1,n2,n3)if(header && header->exist(n1) && header->exist(n2) && header->exist(n3))func(record->getVector3(n1,n2,n3))

	void FeatureRecord::fromTempRecord(TemplateRecord* temprecord)
	{
		OCHeaderInfo* header = temprecord->getHeadInfo();
		
		setID(temprecord->getID());
		SET_INT(setGID, temprecord, "GID");
		SET_INT(setDNO, temprecord, "DNO");
		SET_STRING(setTable, temprecord, "LAYERNAME");
		SET_STRING(setFeatureName, temprecord, "NAME");
		SET_VECTOR3(setOffset, temprecord, "OFFSET_X", "OFFSET_Y", "OFFSET_Z");
		SET_VECTOR3(setScale, temprecord, "SCALE_X", "SCALE_Y", "SCALE_Z");
		SET_STRING(setRotateString, temprecord, "ROTATE");
		SET_STRING(setTime, temprecord, "TIMESTAMP");

		if (header != NULL)
		{
			for (int i=0;i< header->getJoinFieldCount();i++)
			{
				OCJoinFieldInfo* jfield = header->getJoinFieldInfo(i);
				if (jfield->AliasName == "FEATURE_URL")
				{
					setFeatureUrl(temprecord->getFieldValue(jfield->AliasName));
				}
				else if (jfield->AliasName == "TILEID")
				{
					setTileID(temprecord->getIntValue(jfield->AliasName));
				}
				else if (jfield->AliasName == "MODELID")
				{
					setMID(temprecord->getIntValue(jfield->AliasName));
				}
			}
		}

		//
		SET_STRING(setBoundString, temprecord, "BOUNDSPHERE");
		SET_BOOL(setTileModel, temprecord, "TILE");

		getData() = temprecord->getBlobValue("DATA");
	}

	String FeatureRecord::parseBoundString(const osg::Node* node)
	{
		osg::BoundingSphere mBound = node->getBound();
		osg::Vec3 center = mBound.center();
		double radian = mBound.radius();
		String boundSphere = StringConverter::toString(center[0]) + " "
			+ StringConverter::toString(center[1]) + " "
			+ StringConverter::toString(center[2]) + " "
			+ StringConverter::toString(radian);
		return boundSphere;
	}

	bool FeatureRecord::writeNode(const osg::Node* node,String ext,osgDB::Options* options)
	{
		setBoundString(parseBoundString(node));
		
		if (!mDataStream.isNull())
		{
			mDataStream.setNull();
		}

		StringUtil::toLowerCase(ext);

		if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension(ext))
		{
			osg::ref_ptr<osgDB::Options> localOptions;

			if (options == NULL)
			{
				localOptions = new osgDB::Options;
				if (ext == "ive")
				{
					StringStream optionstream;
					optionstream<<"noTexturesInIVEFile"<<std::endl;
					localOptions->setOptionString(optionstream.str());
				}

				if (ext == "osgb")
				{
					StringStream optionstream;
					optionstream<<"WriteImageHint"<<std::endl;
					localOptions->setPluginStringData("WriteImageHint","UseExternal");
				}
			}
			else
			{
				localOptions = options;
			}
			
			StringStream sstream;
			osgDB::ReaderWriter::WriteResult result = rw->writeNode(*node,sstream,localOptions);

			if (result.success())
			{
				mDataStream.bind(new MemoryDataStream(&sstream));
			}
		}

		return !mDataStream.isNull();
	}

	void FeatureRecord::readUserValue(const osg::Object* object)
	{
		if (mBoundString.empty())
			object->getUserValue(String("BoundString"),mBoundString);
		if (mFeatureUrl.empty())
			object->getUserValue(String("FeatureUrl"),mFeatureUrl);
		if (mFeatureName.empty())
			object->getUserValue(String("FeatureName"),mFeatureName);
		object->getUserValue(String("offset_x"),mOffset.x);
		object->getUserValue(String("offset_y"),mOffset.y);
		object->getUserValue(String("offset_z"),mOffset.z);
		object->getUserValue(String("scale_x"),mScale.x);
		object->getUserValue(String("scale_y"),mScale.y);
		object->getUserValue(String("scale_z"),mScale.z);
		if (mGID <0)
		{
			String gidStr;
			object->getUserValue(String("gid"),gidStr);
			mGID = StringConverter::parseLong(gidStr);
		}
		if (mDNO <0)
			object->getUserValue(String("dno"),mDNO);
		if (mTable.empty())
			object->getUserValue(String("table"),mTable);
	}

	void FeatureRecord::writeUserValue(osg::Object* object)
	{
		if (!mBoundString.empty())
			object->setUserValue(String("BoundString"),mBoundString);
		if (!mFeatureUrl.empty())
			object->setUserValue(String("FeatureUrl"),mFeatureUrl);
		if (!mFeatureName.empty())
			object->setUserValue(String("FeatureName"),mFeatureName);
		if (mOffset.x > 0)
			object->setUserValue(String("offset_x"),mOffset.x);
		if (mOffset.y > 0)
			object->setUserValue(String("offset_y"),mOffset.y);
		if (mOffset.z > 0)
			object->setUserValue(String("offset_z"),mOffset.z);
		if (mScale.x > 0)
			object->setUserValue(String("scale_x"),mScale.x);
		if (mScale.y > 0)
			object->setUserValue(String("scale_y"),mScale.y);
		if (mScale.z > 0)
			object->setUserValue(String("scale_z"),mScale.z);
		if (mGID > 0)
		{
			object->setUserValue(String("gid"),StringConverter::toString(mGID));
		}
		if (mDNO > 0)
			object->setUserValue(String("dno"),mDNO);
		if (!mTable.empty())
			object->setUserValue(String("table"),mTable);
	}

	bool FeatureRecord::validData()
	{
		return !mDataStream.isNull() && mDataStream->size() > 0;
	}

	void FeatureRecord::resetData()
	{
		mDataStream.setNull();
	}

	osg::Matrix FeatureRecord::getMatrix()
	{
		CQuaternion quat = StringConverter::parseQuaternion(getRotateString());

		return osg::Matrix::scale(Vector3ToVec3(mScale)) * osg::Matrix::rotate(QuaternionToQuat(quat)) * osg::Matrix::translate(Vector3ToVec3d(mOffset));
	}

	void FeatureRecord::clearEffect()
	{
		
	}

	CFeatureNode* FeatureRecord::setEffect(IFeatureEffect* effect,String featurecode)
	{
		CORE_LOCK_AUTO_MUTEX
		osg::ref_ptr<CFeatureNode> featurenode;
		return featurenode.release();
	}

	osg::Node* FeatureRecord::decorate(osg::Node* in_node)
	{
		osg::ref_ptr<osg::Node> node = in_node;
		return node.release();
	}

	osg::Node* FeatureRecord::readNode(String fileName,const IArchive* archive,const osgDB::ReaderWriter::Options* options)
	{
		return NULL;
	}

	osg::Node* FeatureRecord::getFeatureNode(String code)
	{
		return NULL;
	}

}


