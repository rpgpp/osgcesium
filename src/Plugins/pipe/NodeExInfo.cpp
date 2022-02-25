#include "NodeExInfo.h"
#include "OCModel/ModelUtility.h"
#include "PipeLine.h"
#include "PipeNode.h"
#include "OCModel/IModelTable.h"

namespace OC
{
	namespace Modeling
	{
		NodeExInfo::NodeExInfo(PipeNode* owner):mOwner(owner)
			,mConsDirection(CVector3::ZERO)
			,mGroundZ(0.0f)
		{

		}

		NodeExInfo::~NodeExInfo(void)
		{
		}

		void NodeExInfo::setCategory(String cate)
		{
			mCategory = cate;
		}

		String NodeExInfo::getCategory()
		{
			return mCategory;
		}

		void NodeExInfo::setGroundZ(double z)
		{
			mGroundZ = z;
		}

		double NodeExInfo::getGroundZ()
		{
			return mGroundZ;
		}

		void NodeExInfo::setModelPosition(CVector3 position)
		{
			mModelPosition = position;
		}

		CVector3 NodeExInfo::getModelPosition()
		{
			return mModelPosition;
		}

		CVector3 NodeExInfo::getConstDirection()
		{
			return mConsDirection;
		}

		void NodeExInfo::setConstDirection(CVector3 direction)
		{
			mConsDirection = direction;
		}

		CVector3 NodeExInfo::calcModelDirection()
		{
			//计算本地模型的朝向
			CVector3 direction1;
			CVector3 v1,v2;


			LinePtrList mLinkLines = mOwner->getLinkLines();

			int size = mLinkLines.size();

			int nodeGid = mOwner->getGID();

			if(size == 1)
			{
				Vector3List line1Pt = mLinkLines[0]->getTwoPointFromNode(nodeGid);

				CVector3 p1 = line1Pt[1];
				CVector3 p0 = line1Pt[0];

				if (ProjectTool* tool = mOwner->getOwnerTable()->getProjectTool())
				{
					p1 = tool->worldToLonLatH(p1);
					p0 = tool->worldToLonLatH(p0);

					CVector3 o1 = p1;
					o1.z = 0.0f;

					CVector3 o0 = p0;
					o0.z = 0.0f;

					CVector3 v1 = o1 - o0;
					direction1 = v1;
					if (direction1 == CVector3::ZERO)
					{
						direction1 = CVector3(0, 0, p1.z) - CVector3(0, 0, p0.z);
					}
				}
				else
				{
					CVector3 v1 = p1 - p0;
					direction1 = v1;
				}

				direction1.normalise();
			}
			else if(size == 2)
			{
				Vector3List line1Pt = mLinkLines[0]->getTwoPointFromNode(nodeGid);
				Vector3List line2Pt = mLinkLines[1]->getTwoPointFromNode(nodeGid);

				CVector3 p1 = line1Pt[1];
				CVector3 p0 = line1Pt[0];

				CVector3 o1 = line2Pt[1];
				CVector3 o0 = line2Pt[0];

				if (ProjectTool* tool = mOwner->getOwnerTable()->getProjectTool())
				{
					p1 = tool->worldToLonLatH(p1);
					p0 = tool->worldToLonLatH(p0);
					o1 = tool->worldToLonLatH(o1);
					o0 = tool->worldToLonLatH(o0);
					p1.z = p0.z = o1.z = o0.z = 0.0f;
				}

				CVector3 v1 = (p1 - p0).normalisedCopy();
				CVector3 v2 = (o1 - o0).normalisedCopy();

				if (v1.z < 0)
				{
					v1.z *= -1;
				}
				if (v2.z < 0)
				{
					v2.z *= -1;
				}

				CRadian angle1 = v1.angleBetween(CVector3::UNIT_Z);

				CRadian angle2 = v2.angleBetween(CVector3::UNIT_Z);

				direction1 = angle1 > angle2 ? v1:v2;
			}
			else if(size == 3)
			{
				Vector3List line1Pt = mLinkLines[0]->getTwoPointFromNode(nodeGid);
				CVector3 v1 = (line1Pt[1] - line1Pt[0]).normalisedCopy();
				direction1 = v1;
			}
			else
			{
				if(size > 0)
				{
					Vector3List line1Pt = mLinkLines[0]->getTwoPointFromNode(nodeGid);
					CVector3 v1 = (line1Pt[1] - line1Pt[0]).normalisedCopy();
					direction1 = v1;
				}
			}

			direction1.normalise();

			return direction1;
		}

		ModelNode NodeExInfo::getModelNode()
		{
			return mModelNode;
		}

		void NodeExInfo::setModelNode(ModelNode model)
		{
			mModelNode = model;
		}
	}
}

