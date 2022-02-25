#ifndef _OC_CMPT_WRITER_H
#define _OC_CMPT_WRITER_H

#include "CesiumDefine.h"
#include "FeatureBatchTable.h"
#include "BaseWriter.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport CmptWriter : public osg::Referenced
		{
		public:
			CmptWriter();
			~CmptWriter();

			void push(BaseWriter* object);
			void writeCmpt(String filename);
			uint32 getLength();
		private:
			std::vector<osg::ref_ptr<BaseWriter> >	mTiles;
		};
	}
}

#endif