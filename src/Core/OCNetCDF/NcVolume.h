#ifndef _OC_NetCDF_Volume_H__
#define _OC_NetCDF_Volume_H__

#include "NetCDFDefine.h"
#include <netcdf.h>
#include "OCMain/ConfigManager.h"
#include "OCMain/DataStream.h"
#include "OCUtility/RGBAEncoder.h"
#include "OCVolume/VolumeObject.h"

namespace OC
{
	namespace Volume
	{
		class NcVolume : public CVolumeObject
		{
		public:
			NcVolume();
			virtual ~NcVolume();


			long mDimX = 0;
			long mDimY = 0;
			long mDimZ = 0;
			long mDimension = 3;

			std::vector<int>	elvValues;
			std::vector<float>	var1Values;
			std::vector<float>	var2Values;
			std::vector<float>	lonValues;
			std::vector<float>	latValues;
			std::vector<float>	heiValues;
		};
	}
}

#endif
