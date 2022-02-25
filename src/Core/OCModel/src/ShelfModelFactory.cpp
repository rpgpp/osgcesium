#include "ShelfModelFactory.h"
#include "IModelData.h"

namespace OC
{
	namespace Modeling
	{
		ShelfModelFactory::ShelfModelFactory(void)
		{
		}


		ShelfModelFactory::~ShelfModelFactory(void)
		{
		}

		bool ShelfModelFactory::build(IModelData* data)
		{
			return data->build();
		}
	}
}

