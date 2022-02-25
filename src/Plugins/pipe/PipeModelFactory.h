#ifndef _OC_Model_PipeModelFactory_H__
#define _OC_Model_PipeModelFactory_H__

#include "OCModel/IModelFactory.h"

namespace OC
{
	namespace Modeling
	{
		class PipeModelFactory : public IModelFactory
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
