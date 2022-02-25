#ifndef _OC_B3DM_WRITER_H__
#define _OC_B3DM_WRITER_H__

#include "CesiumDefine.h"
#include "FeatureBatchTable.h"
#include "GLTFObject.h"
#include "BaseWriter.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport B3dmWriter : public BaseWriter
		{
		public:
			B3dmWriter();
			~B3dmWriter();

			virtual bool writeToFile(String filename);
			virtual bool writeToStream(std::ostream& output);
			virtual uint32 bytesLength();

			GLTFObject* getGltfObject();
			void copyAttributes(B3dmWriter* rhs);
			FeatureBatchTable* getFeatureBatchTable();

			std::vector<osg::ref_ptr<B3dmWriter> > splitByCapcity();
		private:
			osg::ref_ptr<GLTFObject>		mGltfObject;
			osg::ref_ptr<FeatureBatchTable> mFeatureBatchTable;
		};
	}
}


#endif

