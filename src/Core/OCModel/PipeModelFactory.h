#ifndef _PIPE_MODEL_FACTORY_CLASS_H__
#define _PIPE_MODEL_FACTORY_CLASS_H__

#include "IModelFactory.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport PipeModelFactory : public IModelFactory
		{
		public:
			PipeModelFactory(void){}
			~PipeModelFactory(void){}

			virtual bool build(IModelData* data);
		public:
			bool createNode(PipeNode* pNode);
			bool createLine(PipeLine* pLine);
		};
	}
}

#endif
