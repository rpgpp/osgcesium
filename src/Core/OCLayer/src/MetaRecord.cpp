#include "MetaRecord.h"
#include "IDatabase.h"

#define TagMeta "Meta"
#define TagSourceType "DataFrom"
#define TagNetName "NetName"
#define TagAliasName "AliasName"
#define TagDBType "DBType"
#define TagExtent "Extent"
#define TagLevel "Level"
#define TagXmin "Xmin"
#define TagYmin "Ymin"
#define TagXmax "Xmax"
#define TagYmax "Ymax"
#define TagTime "TimeStamp"
#define TagInstance "Instance"
#define TagEntityMode "EntityMode"
#define TagGeocenteric "Geocenteric"
#define TagConnectString "ConnectString"
#define TagLineTable "LineTable"
#define TagNodeTable "NodeTable"
#define TagPrj "prj"
#define TagVersion "version"
#define TagEdition "edition"
#define TagMeasureZ "Zflag"

namespace OC
{
	const String CMetaRecord::PipeDatabase = "Pipes";
	const String CMetaRecord::LabelDatabase = "Labels";
	const String CMetaRecord::VectorDatabase = "Vectors";
	const String CMetaRecord::BuildingDatabase = "Buildings";
	const String CMetaRecord::ObliquePhotograph = "ObliquePhotograph";
	const String CMetaRecord::IFCBIM = "IFCBIM";

	CMetaRecord::CMetaRecord(void)
		:mLevel(0)
		,mGeocenteric(false)
		,mInstanceArchive(true)
		,mDataSourceType(DATA_FROM_DB)
		,mDatabaseType(BuildingDatabase)
		,mEntityMode(EntityWithSingleRecord)
		,mDirty(false)
		,mVersion(0)
		,mEdition(0)
	{
		
	}

	CMetaRecord::~CMetaRecord(void)
	{
	}

	void CMetaRecord::setConnectString(String connect)
	{
		mConnectString = connect;
	}

	String CMetaRecord::getConnectString()
	{
		return mConnectString;
	}

	void CMetaRecord::setDBType(String type)
	{
		mDatabaseType = type;
	}

	String CMetaRecord::getDBType()
	{
		return mDatabaseType;
	}

	void CMetaRecord::setDataFrom(ArchiveDataFrom type)
	{
		mDataSourceType = type;
	}

	ArchiveDataFrom CMetaRecord::getDataFrom()
	{
		return mDataSourceType;
	}

	void CMetaRecord::setNetName(String name)
	{
		mNetName = name;
	}

	String CMetaRecord::getNetName()
	{
		return mNetName;
	}

	void CMetaRecord::setAliasName(String name)
	{
		mAliasName = name;
	}

	String CMetaRecord::getAliasName()
	{
		return mAliasName;
	}

	void CMetaRecord::setTimeStamp(String time)
	{
		mTimeStamp = time;
	}

	String CMetaRecord::getTimeStamp()
	{
		return mTimeStamp;
	}

	void CMetaRecord::setLevel(int l)
	{
		mLevel = l;
	}

	int CMetaRecord::getLevel()
	{
		return mLevel;
	}
	
	void CMetaRecord::fromXmlElement(TiXmlElement* root)
	{
		if (root != NULL)
		{
			mNetName = TinyXml::read_attribute(root,TagNetName);
			mTimeStamp = TinyXml::read_attribute(root,TagTime);
			mDatabaseType = TinyXml::read_attribute(root,TagDBType);
			mAliasName = TinyXml::read_attribute(root,TagAliasName);
			mDataSourceType = (ArchiveDataFrom)StringConverter::parseInt(TinyXml::read_attribute(root,TagSourceType));
			mEntityMode = (EntityMode)StringConverter::parseInt(TinyXml::read_attribute(root,TagEntityMode));
			mInstanceArchive = StringConverter::parseBool(TinyXml::read_attribute(root,TagInstance),true);
			mGeocenteric = StringConverter::parseBool(TinyXml::read_attribute(root,TagGeocenteric),false);
			mConnectString = TinyXml::read_attribute(root,TagConnectString);
			mMeasureZ = TinyXml::read_attribute(root,TagMeasureZ);

			if (TiXmlNode* childNode = root->FirstChild(TagExtent))
			{
				mLevel = StringConverter::parseInt(TinyXml::read_attribute(childNode,TagLevel));
				mExtentRect.getMinimum().x = StringConverter::parseReal(TinyXml::read_attribute(childNode,TagXmin));
				mExtentRect.getMinimum().y = StringConverter::parseReal(TinyXml::read_attribute(childNode,TagYmin));
				mExtentRect.getMaximum().x = StringConverter::parseReal(TinyXml::read_attribute(childNode,TagXmax));
				mExtentRect.getMaximum().y = StringConverter::parseReal(TinyXml::read_attribute(childNode,TagYmax));
			}

			if (TiXmlElement* element = root->FirstChildElement(TagLineTable))
			{
				mLineHeader = new OCHeaderInfo;
				mLineHeader->fromXmlElement(element);
			}

			if (TiXmlElement* element = root->FirstChildElement(TagNodeTable))
			{
				mNodeHeader = new OCHeaderInfo;
				mNodeHeader->fromXmlElement(element);
			}

			if (TiXmlElement* element = root->FirstChildElement(TagPrj))
			{
				if (const char* t = element->GetText())
				{
					mSpatialString = t;
				}
			}

			mVersion = StringConverter::parseInt(TinyXml::read_attribute(root,TagVersion));
			mEdition = StringConverter::parseInt(TinyXml::read_attribute(root,TagEdition));
		}
	}
	
	void CMetaRecord::parseXmlString(String xmlString)
	{
		try
		{
			TiXmlDocument doc;
			if (doc.Parse(xmlString.c_str()))
			{
				fromXmlElement(doc.RootElement());
			}
		}
		catch (...){}
	}

	String CMetaRecord::toXmlString()
	{
		TiXmlDocument doc;
		TiXmlElement* element = new TiXmlElement(TagMeta);  
		element->SetAttribute(TagNetName,mNetName);
		element->SetAttribute(TagTime,mTimeStamp);
		element->SetAttribute(TagDBType,mDatabaseType);
		element->SetAttribute(TagAliasName,mAliasName);
		element->SetAttribute(TagSourceType,mDataSourceType);
		element->SetAttribute(TagEntityMode,mEntityMode);
		element->SetAttribute(TagInstance,StringConverter::toString(mInstanceArchive));
		element->SetAttribute(TagGeocenteric,StringConverter::toString(mGeocenteric));
		element->SetAttribute(TagConnectString,mConnectString);
		element->SetAttribute(TagMeasureZ,mMeasureZ);
		element->SetAttribute(TagVersion,mVersion);
		element->SetAttribute(TagEdition,mEdition);

		doc.LinkEndChild(element);  
		TiXmlElement* elementExtent = new TiXmlElement(TagExtent);
		elementExtent->SetAttribute(TagLevel,mLevel);
		elementExtent->SetAttribute(TagXmin,StringConverter::formatDoubletoString(mExtentRect.getMinimum().x,6));
		elementExtent->SetAttribute(TagYmin,StringConverter::formatDoubletoString(mExtentRect.getMinimum().y,6));
		elementExtent->SetAttribute(TagXmax,StringConverter::formatDoubletoString(mExtentRect.getMaximum().x,6));
		elementExtent->SetAttribute(TagYmax,StringConverter::formatDoubletoString(mExtentRect.getMaximum().y,6));
		element->LinkEndChild(elementExtent);

		{
			TiXmlElement* elementPrj = new TiXmlElement(TagPrj);
			TiXmlText* elementText = new TiXmlText(TagPrj);
			elementText->SetValue(mSpatialString);
			elementPrj->LinkEndChild(elementText);
			element->LinkEndChild(elementPrj);
		}

		if (mLineHeader.valid())
		{
			element->LinkEndChild(mLineHeader->toXmlElement(new TiXmlElement(TagLineTable)));
		}

		if (mNodeHeader.valid())
		{
			element->LinkEndChild(mNodeHeader->toXmlElement(new TiXmlElement(TagNodeTable)));
		}

		TiXmlPrinter printer;
		doc.Accept(&printer);
		return printer.CStr();
	}

	void CMetaRecord::setExtent(CRectangle rect)
	{
		mExtentRect = rect;
	}

	CRectangle CMetaRecord::getExtent()
	{
		return mExtentRect;
	}

	CMetaRecord::EntityMode CMetaRecord::getEntityMode()
	{
		return mEntityMode;
	}

	void CMetaRecord::setEntityMode(CMetaRecord::EntityMode mode)
	{
		mEntityMode = mode;
	}

	bool CMetaRecord::isInstanceArchive() const
	{
		return mInstanceArchive;
	}

	void CMetaRecord::setInstanceArchive(bool instance)
	{
		mInstanceArchive = instance;
	}

	bool CMetaRecord::isGeocenteric() const
	{
		return mGeocenteric;
	}

	void CMetaRecord::setGeocenteric(bool geocenteric)
	{
		mGeocenteric = geocenteric;
	}		

	void CMetaRecord::setDatabase(IDatabase* database)
	{
		obDatabase = database;
	}		

	IDatabase* CMetaRecord::getDatabase()
	{
		return obDatabase.get();
	}		

	void CMetaRecord::update(bool force)
	{
		if (force || mDirty)
		{
			osg::ref_ptr<IDatabase> database;
			if (obDatabase.lock(database))
			{
				database->update(this);
			}
		}
	}		

	void CMetaRecord::setDirty()
	{
		mDirty = true;
	}		

	OCHeaderInfo* CMetaRecord::getLineHeader()
	{
		return mLineHeader;
	}		

	void CMetaRecord::setLineHeader(OCHeaderInfo* header)
	{
		mLineHeader = header;
	}		

	OCHeaderInfo* CMetaRecord::getNodeHeader()
	{
		return mNodeHeader;
	}		

	void CMetaRecord::setNodeHeader(OCHeaderInfo* header)
	{
		mNodeHeader = header;
	}		

	void CMetaRecord::setSpatialString(String srs)
	{
		mSpatialString = srs;
	}		

	String CMetaRecord::getSpatialString()
	{
		return mSpatialString;
	}		

	void CMetaRecord::setMeasureZ(String measure)
	{
		mMeasureZ = measure;
	}		

	String CMetaRecord::getMeasureZ()
	{
		return mMeasureZ;
	}		

	int CMetaRecord::getVersion()
	{
		return mVersion;
	}		

	void CMetaRecord::setVersion(int version)
	{
		mVersion = version;
	}		

	int CMetaRecord::getEdition()
	{
		return mEdition;
	}		

	void CMetaRecord::setEdition(int edition)
	{
		mEdition = edition;
	}		

}

