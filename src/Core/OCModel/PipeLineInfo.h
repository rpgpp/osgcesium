#ifndef _PIPE_LINE_INFO_CLASS_H_
#define _PIPE_LINE_INFO_CLASS_H_

#include "PipeLine.h"

namespace OC
{
	namespace Modeling
	{
		class CPipeLineInfo
		{
		public:
			CPipeLineInfo(void);
			~CPipeLineInfo(void);

			LinePtrList& getExtraLine();
		private:
			LinePtrList mLineList;
		};
	}
}


#endif
