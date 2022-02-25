#ifndef _Cesium_TEXTURE_OPTIMIZE_H__
#define _Cesium_TEXTURE_OPTIMIZE_H__

#include "BatchGeode.h"

namespace OC
{
	namespace Cesium
	{
		class TextureOptimize
		{
		public:
			static void merge_clamp(BatchGeodeList& batchGeodeList, BatchGeodeMap& mBatchGeodeMap);
			static void merge_repeat(BatchGeodeList& batchGeodeList, BatchGeodeMap& mBatchGeodeMap);
			static void scaleDimansion(osg::Image* image);
		};
	}
}
#endif

