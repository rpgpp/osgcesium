#include "QuadTree.h"

namespace OC
{
	CQuadTreeNode::CQuadTreeNode(CRectangle extent,int level,CQuadTreeNode* pParent,CQuadTree* pContainer)
		:mContainer(pContainer)
		,mParent(pParent)
		,mHasChildren(false)
	{
		mExtent = extent;
		mLevel = level;
	}

	CQuadTreeNode::~CQuadTreeNode(void)
	{
		if (mHasChildren)
		{
			destroyChildren();
		}

		if (mContainer && mContainer->getAutoDelete())
		{
			std::set<CQuadData*>::iterator it = mQuadDataSet.begin();
			for (;it!=mQuadDataSet.end();it++)
			{
				delete *it;
			}
		}
	}

	int CQuadTreeNode::getR()
	{
		if (mParent)
		{
			for (int r=0;r<2;r++)
			{
				for (int c=0;c<2;c++)
				{
					if (this == mParent->mChildren[r][c])
					{
						return 2 * mParent->getR() + r;
					}
				}
			}
		}

		return 0;
	}

	int CQuadTreeNode::getC()
	{
		if (mParent)
		{
			for (int r=0;r<2;r++)
			{
				for (int c=0;c<2;c++)
				{
					if (this == mParent->mChildren[r][c])
					{
						return 2 * mParent->getC() + c;
					}
				}
			}
		}

		return 0;
	}

	int CQuadTreeNode::getL()
	{
		return mLevel;
	}

	CQuadTreeNode* CQuadTreeNode::getParent()
	{
		return mParent;
	}

	CQuadTree* CQuadTreeNode::getQuadTree()
	{
		return mContainer;
	}

	QuadDataSet CQuadTreeNode::getQuadDataSet()
	{
		return mQuadDataSet;
	}

	void CQuadTreeNode::addQuadData(CQuadData* data,int destLevel)
	{
		if(mExtent.inside(data->getPostion().x,data->getPostion().y))
		{
			if (mLevel == destLevel)
			{
				mQuadDataSet.insert(data);
				data->setQuadNode(this);

				if (mContainer && mContainer->getListener())
				{
					mContainer->getListener()->onAddData(data,this);
				}
			}
			else
			{ 
				if (!mHasChildren)
				{
					partition();
				}
				for (int r=0;r<2;r++)
				{
					for (int c=0;c<2;c++)
					{
						mChildren[r][c]->addQuadData(data,destLevel);
						if (data->getQuadNode() != NULL)
						{
							return;
						}
					}
				}
			}
		}
	}

	void CQuadTreeNode::query(int level,QuadTreeNodeSet& results)
	{
		if (mLevel == level && !mQuadDataSet.empty())
		{
			results.insert(this);
		}
		else if(mHasChildren)
		{
			for (int r=0;r<2;r++)
			{
				for (int c=0;c<2;c++)
				{
					mChildren[r][c]->query(level,results);
				}
			}
		}
	}

	void CQuadTreeNode::destroyChildren()
	{
		for(int r = 0; r < 2; r++)
			for(int c = 0; c < 2; c++)
				delete mChildren[r][c];
	}

	void CQuadTreeNode::partition()
	{
		mHasChildren = true;
		for (int r=0;r<2;r++)
		{
			for (int c=0;c<2;c++)
			{
				CVector2 halfWidth = (mExtent.getMaximum() - mExtent.getMinimum()) * 0.5;

				CRectangle extent;
				extent.setMinimum(mExtent.getMinimum().x + halfWidth.x * c,mExtent.getMinimum().y + halfWidth.y * r);
				extent.setMaximum(extent.getMinimum().x + halfWidth.x,extent.getMinimum().y + halfWidth.y);

				mChildren[r][c] = new CQuadTreeNode(extent,mLevel + 1,this,mContainer);
			}
		}
	}

	CRectangle CQuadTreeNode::getExtent()
	{
		return mExtent;
	}

	CQuadTree::CQuadTree(CRectangle extent,int level)
		:mLevelDepth(level)
		,mAutoDeleteData(false)
		,mListener(NULL)
	{
		mLevelDepth = Math::Clamp(level,1,16);
		mRootNode = new CQuadTreeNode(extent,1,NULL,this);
	}

	CQuadTree::~CQuadTree(void)
	{
		delete mRootNode;
	}

	void CQuadTree::appendData(CQuadData* data)
   	{
		mRootNode->addQuadData(data,mLevelDepth);
	}

	CQuadTreeNode* CQuadTree::getRoot()
	{
		return mRootNode;
	}

	QuadTreeNodeSet CQuadTree::query()
	{
		QuadTreeNodeSet set;
		mRootNode->query(mLevelDepth,set);
		return set;
	}

	void CQuadTree::setAutoDelete(bool del)
	{
		mAutoDeleteData = del;
	}

	bool CQuadTree::getAutoDelete()
	{
		return mAutoDeleteData;
	}

	int CQuadTree::getDepth()
	{
		return mLevelDepth;
	}

	void CQuadTree::setListner(CQuadTreeListener* listner)
	{
		mListener = listner;
	}

	CQuadTreeListener* CQuadTree::getListener()
	{
		return mListener;
	}

	CRectangle CQuadTree::calcTileExtent(int r,int c)
	{
		return calcTileExtent(r,c,mLevelDepth);
	}

	CRectangle CQuadTree::calcTileExtent(int r,int c,int l)
	{
		CRectangle rect;

		CRectangle mRootExtent = mRootNode->getExtent();

		int totalR = (int)Math::Pow(2,(float)(l - 1));
		double perDisX = (mRootExtent.getMaximum().x - mRootExtent.getMinimum().x)/(double)totalR;
		double perDisY = (mRootExtent.getMaximum().y - mRootExtent.getMinimum().y)/(double)totalR;

		double minx = mRootExtent.getMinimum().x + perDisX * c;
		double ymin = mRootExtent.getMinimum().y + perDisY * r;
		double xmax = minx + perDisX;
		double ymax = ymin + perDisY;

		rect.setExtents(CVector2(minx,ymin),CVector2(xmax,ymax));

		return rect;
	}

	bool CQuadTree::calcTileofModel(const CVector2 &center,int &r,int &c)
	{
		CRectangle extent = mRootNode->getExtent();
		//if(extent.inside(center.x,center.y))
		{
			int totalR = (int)Math::Pow(2,(float)(mLevelDepth - 1));
			double perDisX = (extent.getMaximum().x - extent.getMinimum().x)/(double)totalR;
			double perDisY = (extent.getMaximum().y - extent.getMinimum().y)/(double)totalR;
			CVector2 offset = center - extent.getMinimum();//.x/perDisX
			c = Math::Clamp(int(offset.x/perDisX),0,totalR - 1);
			r = Math::Clamp(int(offset.y/perDisY),0,totalR - 1);
			return true;
		}
		//return false;
	}
}
