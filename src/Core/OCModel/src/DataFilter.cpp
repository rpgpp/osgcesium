#include "DataFilter.h"
#include "PipeLine.h"
#include "PipeNode.h"

namespace OC
{
	namespace Modeling
	{
		CDataFilter::CDataFilter(void)
		{
		}

		CDataFilter::~CDataFilter(void)
		{
		}

		void CDataFilter::setTimeClause(String time)
		{
			mTimeClause = time;
		}

		String CDataFilter::getTimeClause()
		{
			return mTimeClause;
		}

		CRectangle CDataFilter::getExtent()
		{
			return mExtent;
		}

		void CDataFilter::setExtent(CRectangle rect)
		{
			mExtent = rect;
		}

		longVector& CDataFilter::getNodeIDList()
		{
			return mNodeIDList;
		}

		longVector& CDataFilter::getLineIDList()
		{
			return mLineIDList;
		}

		void CDataFilter::setLineWhereClause(String clause)
		{
			mLineClause = clause;
		}

		String CDataFilter::getLineWhereClause()
		{
			return mLineClause;
		}

		void CDataFilter::setNodeWhereClause(String clause)
		{
			mNodeClause = clause;
		}

		String CDataFilter::getNodeWhereClause()
		{
			return mNodeClause;
		}
	}
}


