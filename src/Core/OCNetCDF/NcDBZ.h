#ifndef _OC_Nc_DBZ_H__
#define _OC_Nc_DBZ_H__

#include "NcVolume.h"

namespace OC
{
	namespace Volume
	{
		class NcDBZ : public NcVolume
		{
		public:
			NcDBZ(String varname);
			~NcDBZ();
			virtual bool convert(std::string filename);

			String mVarname;
			float fill_value = -31.328f;
			float missing_value = -31.328f;
			float unit = 1000.0f;
		};
	}
}


#endif