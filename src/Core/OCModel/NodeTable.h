#ifndef _NODE_TABLE_CLASS_H_
#define _NODE_TABLE_CLASS_H_

#include "PipeNode.h"
#include "QuadTreeGenerator.h"
#include "DataFilter.h"

namespace OC
{
	namespace Modeling
	{
		
		class _ModelExport NodeTable
		{
		public:
			NodeTable(PipeTable* owner);
			~NodeTable();

			NodeMap& getNodeMap();
			NodeMapIterator getIterator(void);

			void push(PipeNode* node);
			long findID(String nodecode,CVector3 position);
			PipeNode* find(long gid);

			bool checkData();

			void fixData();

			void clear();
			
			String getNameTable();
			void setNameTable(String table);

			String getAliasName();
			void setAliasName(String name);

			CRectangle& getExtent();

			void removeInvalidData();

			IModelDataList getBlockMap();

			std::multimap<String,long>& getCodeMap(){return mCodeIDMap;}

			osg::ref_ptr<CDataFilter>& getFilter(){return mDataFilter;}
		protected:
			NodeMap							mNodeList;
			std::multimap<String,long>		mCodeIDMap;
		private:
			String							mAliasName;
			String							mTableName;
			PipeTable*						mOwner;
			CRectangle						mExtent;
			osg::ref_ptr<CDataFilter>		mDataFilter;

			inline void doModelType(PipeNode* node);
			inline void doFeatureInfo(PipeNode* node);
			inline void doMaterialInfo(PipeNode* node);
		};
	}
}


#endif



