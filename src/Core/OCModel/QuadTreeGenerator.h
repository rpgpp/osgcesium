#ifndef _OCMODEL_QUADTREE_GENERATOR_CLASS_H_
#define _OCMODEL_QUADTREE_GENERATOR_CLASS_H_

#include "OCLayer/QuadTree.h"
#include "IModelData.h"
#include "IModelTable.h"

namespace OC
{
	namespace Modeling
	{
		class PipeQuadData : public CQuadData
		{
		public:
			PipeQuadData(ISimpleModelData* data)
			{
				mModelData = data;

				CVector3 center3 = data->getCenter();

				
				if (ProjectTool* project = data->getOwnerTable()->getProjectTool())
				{
					center3 = data->getWorldCenter();
				}
				CVector2 position = CVector2(center3.x, center3.y);

				setPosition(position);
			}
			~PipeQuadData(){}

			ISimpleModelData* get()
			{
				return mModelData;
			}
		private:
			ISimpleModelData* mModelData;
		};

		class CQuadTreeGenerator
		{
		public:
			CQuadTreeGenerator(CRectangle extent,int level)
				:mQuadTree(extent,level)
			{
				mQuadTree.setAutoDelete(true);
			}
			~CQuadTreeGenerator(){}

			void append(ISimpleModelData* data)
			{
				mQuadTree.appendData(new PipeQuadData(data));
			}

			IModelDataList generate(IModelTable* owner,String table)
			{
				std::map<String,int> mKeys;

				IModelDataList list;
				QuadTreeNodeSet qNodeSet = mQuadTree.query();
				for (QuadTreeNodeSet::iterator it = qNodeSet.begin();it!=qNodeSet.end();it++)
				{
					CQuadTreeNode* pQuadNode = (*it);

					CRectangle extent = pQuadNode->getExtent();

					IGroupModelData* g = new IGroupModelData;
					g->setOwnerTable(owner);
					g->setRow(pQuadNode->getR());
					g->setCol(pQuadNode->getC());
					g->setLevel(mQuadTree.getDepth());
					g->setTable(table);

					String tilename = g->generateName();
					g->setFeatureName(tilename);
					g->setFeatureUrl(tilename);

					CVector2 dimantion = extent.getMaximum() - extent.getMinimum();
					CVector2 center = extent.getMinimum() + dimantion * 0.5f;

					g->setBoundString(StringConverter::toString(CVector4(0,0,0,dimantion.length())));

					g->setCenter(CVector3(center.x,center.y,0));

					QuadDataSet dModelSet = pQuadNode->getQuadDataSet();
					for(QuadDataSet::iterator jt = dModelSet.begin();jt!=dModelSet.end();jt++)
					{
						PipeQuadData* d = (PipeQuadData*)(*jt);
						g->getList().push_back(d->get());

#if 0
						CQuadTreeNode* p = d->getQuadNode()->getParent();
						while (p)
						{
							StringStream sstream;
							sstream << p->getL() << "_" << p->getR() << "_" << p->getC();
							String key = sstream.str();
							mKeys[key]++;
							p = p->getParent();
						}
#endif
					}

					list.push_back(g);
				}

				std::map<String,int>::iterator it = mKeys.begin();
				for (;it!=mKeys.end();it++)
				{
					IGroupModelData* g = new IGroupModelData;
					StringVector sv = StringUtil::split(it->first,"_");
					g->setRow(StringConverter::parseInt(sv[1]));
					g->setCol(StringConverter::parseInt(sv[2]));
					g->setLevel(StringConverter::parseInt(sv[0]));
					list.insert(list.begin(),1,g);
				}

				return list;
			}

		private:
			CQuadTree mQuadTree;
		};
	}
}

#endif