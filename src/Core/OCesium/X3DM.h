#ifndef OC_X3DM_WRITER_H
#define OC_X3DM_WRITER_H

#include "CesiumDefine.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport X3DMWriter
		{
		public:
			static bool writePNTS(const String& location,
				const String& featureTableJSON,
				const String& featureTableBody,
				const String& batchTableJSON,
				const String& batchTableBody);
		};
	}
}

#endif // OSGEARTH_B3DM_WRITER_H