#ifndef _SHELF_MODEL_FACTORY_H_
#define _SHELF_MODEL_FACTORY_H_

#include "IModelFactory.h"

namespace OC
{
	namespace Modeling
	{
		class ShelfModelFactory : public IModelFactory
		{
		public:
			ShelfModelFactory(void);
			~ShelfModelFactory(void);
			virtual bool build(IModelData* data);
		};
	}
}



#endif
