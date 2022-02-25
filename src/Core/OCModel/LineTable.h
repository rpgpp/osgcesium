#ifndef _LINE_TABLE_CLASS_H_
#define _LINE_TABLE_CLASS_H_

#include "PipeLine.h"
#include "QuadTreeGenerator.h"
#include "DataFilter.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport LineTable
		{
		public:
			LineTable(PipeTable* owner);
			~LineTable();

			LineMap& getLineMap(){return mLineList;}
			LineMapIterator getIterator(void);

			void push(PipeLine* line);
			PipeLine* find(long gid);

			void fixData();

			bool checkData();

			void clear();

			CRectangle& getExtent();

			String getNameTable();
			void setNameTable(String table);

			String getAliasName();
			void setAliasName(String name);

			void removeInvalidData();

			IModelDataList getBlockMap();

			osg::ref_ptr<CDataFilter>& getFilter(){return mDataFilter;}
		protected:
			inline void doFixLineZ(PipeLine* line);
			inline void doFeatureInfo(PipeLine* line);
			inline void doMaterialInfo(PipeLine* line);
			
			//管段减去一段距离处理
			inline void doSubtract();
			inline void doGD_Strategy();
			inline void doJS_Strategy();
			String								mAliasName;
			String								mTableName;
			LineMap								mLineList;
			PipeTable*							mOwner;
			CRectangle							mExtent;
			osg::ref_ptr<CDataFilter>			mDataFilter;
		};
	}
}


#endif



