#ifndef _OC_BZip2_H__

#include "ZipDefine.h"
#include <sstream>

namespace OC
{
	class ZEXPORT2 BZip2
	{
	public:
		BZip2() {}
		~BZip2() {}

		static size_t openBZ2(std::string filename, std::ostream& os);
	};
}

#endif // !_OC_BZip2_H__

