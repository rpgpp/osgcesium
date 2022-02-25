#ifndef _OC_GLTF_OBJCET_H_
#define _OC_GLTF_OBJCET_H_

#include "BatchGroup.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport GLTFObject : public BatchGroup
		{
		public:
			GLTFObject(FeatureBatchTable* featureBatchTable = NULL);

			bool writeToFile(String filename, bool isBinary);
			bool writeToStream(std::ostream& output, bool isBinary);
			uint32 bytesLength();
		protected:
			~GLTFObject();
		private:
			uint32							mBytesLength = 0;
		};
	}
}


#endif
