#ifndef _META_RECORD_CLASS_H__
#define _META_RECORD_CLASS_H__

#include "OCMain/DatabaseDefine.h"
#include "OCMain/BaseEnum.h"
#include "LayerDefine.h"

namespace OC
{
	class _LayerExport CMetaRecord : public osg::Referenced
	{
	public:
		enum EntityMode
		{
			EntityWithSingleRecord,
			EntityWithMutiRecord,
			EntityWithTileOptimize
		};
	public:
		static const String PipeDatabase;
		static const String LabelDatabase;
		static const String VectorDatabase;
		static const String BuildingDatabase;
		static const String ObliquePhotograph;
		static const String IFCBIM;
	public:
		CMetaRecord(void);
		~CMetaRecord(void);

		void fromXmlElement(TiXmlElement* element);
		void parseXmlString(String xmlString);
		String toXmlString();

		void setDataFrom(ArchiveDataFrom type);
		ArchiveDataFrom getDataFrom();

		void setDBType(String type);
		String getDBType();

		void setNetName(String name);
		String getNetName();

		void setAliasName(String name); 
		String getAliasName();

		void setLevel(int l);
		int getLevel();

		void setTimeStamp(String time);
		String getTimeStamp();

		void setExtent(CRectangle rect);
		CRectangle getExtent();

		EntityMode getEntityMode();
		void setEntityMode(EntityMode mode);

		bool isInstanceArchive() const;
		void setInstanceArchive(bool instance);

		bool isGeocenteric() const;
		void setGeocenteric(bool geocenteric);

		void setDatabase(IDatabase* database);
		IDatabase* getDatabase();

		void setConnectString(String connect);
		String getConnectString();

		void update(bool force = false);

		void setDirty();

		OCHeaderInfo* getLineHeader();
		void setLineHeader(OCHeaderInfo* header);

		OCHeaderInfo* getNodeHeader();
		void setNodeHeader(OCHeaderInfo* header);

		void setSpatialString(String srs);
		String getSpatialString();

		void setMeasureZ(String measure);
		String getMeasureZ();

		int getVersion();
		void setVersion(int version);

		int getEdition();
		void setEdition(int edition);
	protected:
		int												mVersion;
		int												mEdition;
		int												mLevel;
		bool												mInstanceArchive;
		bool												mGeocenteric;
		String											mMeasureZ;
		String											mSpatialString;
		String											mNetName;
		String											mTimeStamp;
		String											mAliasName;
		String											mDatabaseType;
		String											mConnectString;
		double											mLodRange;
		EntityMode									mEntityMode;
		CRectangle									mExtentRect;
		ArchiveDataFrom							mDataSourceType;
		osg::observer_ptr<IDatabase>		obDatabase;
		osg::ref_ptr<OCHeaderInfo> mNodeHeader;
		osg::ref_ptr<OCHeaderInfo> mLineHeader;
	private:
		bool							mDirty;
	};
}


#endif


