#ifndef _OC_QUADTREE_CLASS__
#define _OC_QUADTREE_CLASS__


#include "OCUtility/Rectangle.h"
#include "LayerDefine.h"

namespace OC
{
	class CQuadData;
	class CQuadTreeNode;
	typedef std::set<CQuadData*> QuadDataSet;
	typedef std::set<CQuadTreeNode*> QuadTreeNodeSet;
	
	class _LayerExport CQuadTreeListener
	{
	public:
		virtual void onAddData(CQuadData* data,CQuadTreeNode* quadnode) = 0;
	};

	class _LayerExport CQuadData
	{
	public:
		CQuadData()
			:mContainer(NULL)
		{

		}

		virtual ~CQuadData(){}
		
		void setPosition(CVector2 position)
		{
			mPosition = position;
		}

		CVector2 getPostion()
		{
			return mPosition;
		}

		void setQuadNode(CQuadTreeNode* node)
		{
			mContainer = node;
		}

		CQuadTreeNode* getQuadNode()
		{
			 return mContainer;
		}
	private:
		CVector2		mPosition;
		CQuadTreeNode*	mContainer;
	};

	class _LayerExport CQuadTreeNode
	{
	public:
		CQuadTreeNode(CRectangle extent,int level,CQuadTreeNode* pParent = NULL,CQuadTree* pContainer = NULL);
		~CQuadTreeNode(void);

		int getR();
		int getC();
		int getL();
		CQuadTree* getQuadTree();
		CQuadTreeNode* getParent();

		void partition();
		void destroyChildren();

		void addQuadData(CQuadData* data,int destLevel);
		QuadDataSet getQuadDataSet();

		CRectangle getExtent();

		void query(int level,QuadTreeNodeSet& results);
	private:
		int						mLevel;
		bool					mHasChildren;
		CRectangle				mExtent;
		CQuadTree*				mContainer;
		QuadDataSet				mQuadDataSet;
		CQuadTreeNode*			mParent;
		CQuadTreeNode*			mChildren[2][2];
	};

	class _LayerExport CQuadTree : public osg::Referenced
	{
	public:
		CQuadTree(CRectangle extent,int level);
		~CQuadTree(void);

		void setListner(CQuadTreeListener* listner);
		CQuadTreeListener* getListener();

		void appendData(CQuadData* data);

		CQuadTreeNode* getRoot();

		QuadTreeNodeSet query();

		void setAutoDelete(bool del);
		bool getAutoDelete();
		int getDepth();

		CRectangle calcTileExtent(int r,int c);
		CRectangle calcTileExtent(int r,int c,int l);
		bool calcTileofModel(const CVector2 &center,int &r,int &c);
	private:
		int					mLevelDepth;
		bool				mAutoDeleteData;
		CQuadTreeNode*		mRootNode;
		CQuadTreeListener*	mListener;
	};

}

#endif
