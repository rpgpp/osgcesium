#ifndef _OC_MODEL_QUERY_H__
#define _OC_MODEL_QUERY_H__

#include "ModelDefine.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport ModelDataQueryCallback : public CQueryCallback
		{
		public:
			enum DataType
			{
				Type_Line,
				Type_Node,
				Type_Mold
			};
			ModelDataQueryCallback(IModelTable* table,DataType type);
			~ModelDataQueryCallback();
			virtual bool onQuery(TemplateRecord* record);
		private:
			IModelTable*		mTable;
			DataType			mType;
		};
	}
}

#endif

