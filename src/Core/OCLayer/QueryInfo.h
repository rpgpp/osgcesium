#ifndef _OC_QUERY_INFO_CLASS_H__
#define _OC_QUERY_INFO_CLASS_H__

#include "LayerDefine.h"
#include "OCMain/CoreConfig.h"

namespace OC
{
	class _LayerExport CQueryCallback : public osg::Referenced
	{
	public:
		CQueryCallback(){}
		~CQueryCallback(){}

		virtual bool onQuery(TemplateRecord* record){return true;}
	};
	class _LayerExport CQueryInfo : public osgDB::ReaderWriter::Options , public CCoreConfig
	{
	public:
		CQueryInfo(void);
		CQueryInfo(const CQueryInfo&,const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
		virtual ~CQueryInfo(void);

		META_Object(OC,CQueryInfo)

		static String NameTileID;
		static String NameClause;
		static String NameRectangle;
		static String NameLayerName;
		static String NamePaperRange;
		static String NameCreateTileRecord;
		static String NameQueryModelData;

		void setCallback(CQueryCallback* query);
		CQueryCallback* getCallback();
	private:
		osg::ref_ptr<CQueryCallback> mCallback;
	};

}

#endif
