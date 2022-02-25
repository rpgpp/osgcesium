#include "QueryInfo.h"

namespace OC
{
	String CQueryInfo::NameTileID = "TileID";
	String CQueryInfo::NameClause = "Clause";
	String CQueryInfo::NameRectangle = "Rectangle";
	String CQueryInfo::NameLayerName = "LayerName";
	String CQueryInfo::NamePaperRange = "PaperRange";
	String CQueryInfo::NameCreateTileRecord = "CreateTileRecord";
	String CQueryInfo::NameQueryModelData = "QueryModelData";

	CQueryInfo::CQueryInfo(void)
	{
	}

	CQueryInfo::CQueryInfo(const CQueryInfo& info,const osg::CopyOp& copyop)
		:CCoreConfig(info)
	{
	}

	CQueryInfo::~CQueryInfo(void)
	{
	}

	void CQueryInfo::setCallback(CQueryCallback* query)
	{
		mCallback = query;
	}

	CQueryCallback* CQueryInfo::getCallback()
	{
		return mCallback.get();
	}

}
