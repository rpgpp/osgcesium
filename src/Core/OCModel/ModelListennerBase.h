#pragma once
#include "ModelDefine.h"
#include "OCMain/Environment.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport ModelListennerBase
		{
		public:
			ModelListennerBase(void);
			~ModelListennerBase(void);
		public:
			virtual void setProcessFocus(int n,int num){};
			virtual void setEditText(String text){};
		};
	}
}
