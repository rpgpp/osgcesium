#ifndef _OC_Volume_NcZT_H__
#define _OC_Volume_NcZT_H__

#include "NcVolume.h"

namespace OC
{
	namespace Volume
	{
		class NcZT : public NcVolume
		{
		public:
			NcZT(String varname);
			~NcZT();
			virtual bool convert(std::string filename);
		private:
			String mVarname;
		};
	}
}

#endif
