#ifndef _OC_GDALTOOL_H__

#include "OCMain/Environment.h"

namespace OC
{
	class GDALTool
	{
	public:
		GDALTool();
		~GDALTool();
		void testIDW(String filename);
		void testContour(String inputDEM,String outputSHP,double dfContourInterval);
	};
}

#endif // !_OC_GDALTOOL_H__


