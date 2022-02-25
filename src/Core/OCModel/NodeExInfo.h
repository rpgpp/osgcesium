#ifndef _NODE_EX_INFO_H
#define _NODE_EX_INFO_H

#include "ModelDefine.h"
#include "Modelnode.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport NodeExInfo
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
