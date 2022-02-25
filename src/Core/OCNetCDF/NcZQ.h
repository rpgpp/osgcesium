#ifndef _OC_Volume_NcZQ_H__
#define _OC_Volume_NcZQ_H__

#include "NcVolume.h"

namespace OC
{
	namespace Volume
	{
		class NcZQ : public NcVolume
		{
		public:
			NcZQ(String varname);
			~NcZQ();

			virtual bool convert(std::string filename);
		private:
			String mVarname;
		};
	}
}

#endif


