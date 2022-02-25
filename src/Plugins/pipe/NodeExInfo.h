#ifndef _OC_Model_NodeExInfo_H__
#define _OC_Model_NodeExInfo_H__

#include "OCModel/Modelnode.h"

namespace OC
{
	namespace Modeling
	{
		class NodeExInfo
		{
		public:
			NodeExInfo(PipeNode* owner);
			~NodeExInfo(void);

			CVector3 calcModelDirection();

			CVector3 getConstDirection();
			void setConstDirection(CVector3 direction);

			ModelNode getModelNode();
			void setModelNode(ModelNode model);

			void setModelPosition(CVector3 position);
			CVector3 getModelPosition();

			void setGroundZ(double z);
			double getGroundZ();

			void setCategory(String cate);
			String getCategory();
		private:
			double			mGroundZ;
			String			mCategory;
			CVector3		mConsDirection;
			CVector3		mModelPosition;
			PipeNode*		mOwner;
			ModelNode		mModelNode;
		};
	}
}




#endif
