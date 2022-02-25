#ifndef _IMODEL_FACTORY_CLASS_H_
#define _IMODEL_FACTORY_CLASS_H_

#include "ModelDefine.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport IModelFactory
		{
		public:
			IModelFactory(void);
			virtual	 ~IModelFactory(void);

			virtual bool build(IModelData* data){return false;}
		};
	}
}



#endif

