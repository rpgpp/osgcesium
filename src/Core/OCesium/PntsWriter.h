#ifndef _OC_PNTS_WRITER_H
#define _OC_PNTS_WRITER_H

#include "BaseWriter.h"
#include "FeatureBatchTable.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport PntsWriter : public BaseWriter
		{
		public:
			PntsWriter();
			~PntsWriter();
			void push(osg::Vec3 pos);
			void push_normal(osg::Vec3 normal);
			void push_color(uint8 r, uint8 g, uint8 b);
			void push_color(osg::Vec3ub color);
			void push_positionAndBatchId(osg::Vec3 pos, int batchId);

			virtual bool writeToFile(String filename);
			virtual bool writeToStream(std::ostream& output);

			osg::ref_ptr<osg::Vec3Array>			positions;
			osg::ref_ptr<osg::Vec3Array>			normals;
			osg::ref_ptr<osg::UIntArray>			batchIdArray;
			osg::ref_ptr<osg::Vec3ubArray>			colors;
			uint32									batch_length = 0;
			FeatureBatchTable						mFeatureBatchTable;
		};
	}
}

#endif

