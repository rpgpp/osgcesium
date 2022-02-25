#ifndef _OC_VOLUME_VTK_H__
#define _OC_VOLUME_VTK_H__

#include "VolumeObject.h"

namespace OC
{
	namespace Volume
	{
		class _VoloumeExport Vtk : public CVolumeObject
		{
		public:
			Vtk(String varname = "xyz");
			~Vtk();
			bool	convert(std::string filename);
			bool	mNeedStatistic;
			bool	mUseXYZ;
			bool	mConvertGauss;
			float	mFillValue;
			String	mVarname;
		};
	}
}

#endif // !_OC_VOLUME_VTK_H__


