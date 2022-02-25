#ifndef _OC_NETCDF_DEFINE_H_
#define _OC_NETCDF_DEFINE_H_

#if defined(_MSC_VER)
#ifndef OCNetCDF_EXPORTS
#define _NetCDFExport  __declspec(dllimport)
#else
#define _NetCDFExport  __declspec(dllexport)
#endif
#else
#define _NetCDFExport
#endif

#include "OCMain/osg.h"
#include "OCUtility/StringUtil.h"
#include "OCUtility/Rectangle.h"
#include "OCUtility/StringConverter.h"

namespace OC
{
	class CNetCDFReader;

#define NcCheck(e) if(NC_NOERR != e) {printf("Error: %s\n", nc_strerror(e)); return false;}

}

#endif // 
