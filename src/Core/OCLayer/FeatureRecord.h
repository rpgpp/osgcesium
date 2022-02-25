#ifndef _FEATURE_RECORD_CLASS_H__
#define _FEATURE_RECORD_CLASS_H__

#include "OCMain/DataStream.h"
#include "OCMain/ObjectContainer.h"
#include "OCUtility/Sphere.h"
#include "IDatabase.h"
#include "IRecord.h"

#include "LayerDefine.h"

namespace OC
{
	class _LayerExport FeatureRecord : public IRecord
	{
	public:
		class _LayerExport FeatureRecordNodeDecorator : public osg::Referenced
		{
		public:
			virtual osg::Node* onDecorate(FeatureRecord* record,osg::Node* in_node) = 0;
		};
	public:
		FeatureRecord();
		virtual ~FeatureRecord();
		
		virtual void onInsert(){}
		virtual void onUpdate(){}

		TemplateRecord* toTemplateRecord(OCHeaderInfo* hd = NULL);
		void fromTempRecord(TemplateRecord* temprecord);
		static String parseBoundString(const osg::Node* node);
		static String makeCode(String gid,String table);
		static String makeCode(long gid,String table);
		String getCode();

		CSphere getBoundSphere();
		String getBoundString();
		void setBoundString(String name); 

		String getRotateString();
		void setRotateString(String name);

		String getMaterialString();
		void setMaterialString(String name);

		String getFeatureUrl();
		void setFeatureUrl(String name);

		CVector3 getOffset();
		void setOffset(CVector3 offset);

		CVector3 getScale();
		void setScale(CVector3 scale);

		void setDNO(int dno);
		int getDNO();

		void setGID(int gid);
		long getGID();

		void setMID(long mid);
		long getMID();

		void setTileID(int tid);
		int getTileID();

		String getTable();
		void setTable(String table);

		String getTime();
		void setTime(String timeStamp);

		IDatabase* getDatabase();
		void setDatabase(IDatabase* oper);

		MemoryDataStreamPtr& getData();

		void readUserValue(const osg::Object* object);
		void writeUserValue(osg::Object* object);

		bool validData();
		void resetData();

		bool isTileModel();
		void setTileModel(bool isTile);

		osg::Matrix getMatrix();

		virtual bool writeNode(const osg::Node* node,String ext,osgDB::Options* options = NULL);
		virtual osg::Node* readNode(const osgDB::Options* options = NULL);
		virtual osg::Node* readNode(String filename,const IArchive* archive,const osgDB::ReaderWriter::Options* options = NULL);

		virtual CFeatureNode* setEffect(IFeatureEffect* effect,String featurecode);
		void clearEffect();

		osg::Node* getFeatureNode(String code);
	protected:
		osg::Node* decorate(osg::Node* node);
		RefObjectCacher<IFeatureEffect>					mFeatureEffectInfo;
		CORE_AUTO_MUTEX
	private:
		int							 mDNO;
		int							 mTileID;
		long						 mGID;
		long						 mModelID;
		bool						 mIsTile;

		String						 mTable;
		String						 mTimeStamp;
		String						 mFeatureUrl;
		String						 mBoundString;
		String						 mRotateString;
		String						 m_strMaterialString;
									
		CVector3					 mScale;
		CVector3					 mOffset;
		MemoryDataStreamPtr			 mDataStream;
		osg::observer_ptr<IDatabase> mDatabase;
	};

	typedef std::vector<osg::ref_ptr<FeatureRecord> > FeatureRecordList;
}

#endif
