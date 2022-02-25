
#include "PipeLineInfo.h"

namespace OC
{
	namespace Modeling
	{
		CPipeLineInfo::CPipeLineInfo(void)
		{
		}


		CPipeLineInfo::~CPipeLineInfo(void)
		{
		}

		LinePtrList& CPipeLineInfo::getExtraLine()
		{
			return mLineList;
		}
	}
}

