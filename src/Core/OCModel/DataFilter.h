#ifndef _DATA_FILTER_CLASS_H__
#define _DATA_FILTER_CLASS_H__

#include "ModelDefine.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport CDataFilter :public osg::Referenced
		{
		public:
			CDataFilter(void);
			~CDataFilter(void);

			void setTimeClause(String time);
			String getTimeClause();

			void setExtent(CRectangle rect);
			CRectangle getExtent();

			longVector& getNodeIDList();
			longVector& getLineIDList();

			void setNodeWhereClause(String clause);
			String getNodeWhereClause();

			void setLineWhereClause(String clause);
			String getLineWhereClause();
		private:
			String			mTimeClause;
			String			mLineClause;
			String			mNodeClause;
			CRectangle		mExtent;
			longVector		mNodeIDList;
			longVector		mLineIDList;
		};
	}
}



#endif


