#include "ModelDataQueryCallback.h"
#include "PipeLine.h"
#include "PipeNode.h"
#include "IModelTable.h"

namespace OC
{
	namespace Modeling
	{
		ModelDataQueryCallback::ModelDataQueryCallback(IModelTable* table,ModelDataQueryCallback::DataType type)
			:mTable(table)
			,mType(type)
		{

		}
		ModelDataQueryCallback::~ModelDataQueryCallback()
		{

		}

		bool ModelDataQueryCallback::onQuery(TemplateRecord* record)
		{
			IModelData* data = NULL;

			switch(mType)
			{
			case Type_Line:
				{
					data = PipeLine::create(record);
					break;
				}
			case Type_Node:
				{
					data = PipeNode::create(record);
					break;
				}
			case Type_Mold:
				{
					//data = GenericMoldTable::createMoldData(mTable->getName(),record);
					break;
				}
			default:
				break;
			}

			if (data != NULL)
			{
				mTable->push_data(data);
			}

			return data != NULL;
		}

	}
}
