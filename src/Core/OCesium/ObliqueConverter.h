#ifndef _Oblique_CONVETER_H__
#define _Oblique_CONVETER_H__

#include "glTFOptimize.h"
#include "ConvertWorkqueue.h"
#include "MapProject.h"
#include "OCesium/CesiumTool.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport ObliqueConverter
		{
		public:
			ObliqueConverter(int threadNum = 10);
			~ObliqueConverter();
			bool convert(String srcDir, String destDir, String file, int level, OC::Cesium::Tile* parentTile, osg::Node* parentNode);
			void readNode(String filename);
			bool mRWriteB3dm = false;
			bool mRWriteJson = false;
			bool resumeMode = false;
			bool mergeTop = false;
			int mMaxTileNodeNum = 32;
			int mMaxRootTileNodeNum = 64;
			int mDebugDepth = 100;
			String mDestDir;
			CVector3 mLocation = CVector3(108.9594, 34.2196, 0.0);
			osg::BoundingBox mRootBoundBox;
			CORE_AUTO_MUTEX

				class ObliqueAttributeCallback : public Cesium::FeatureAttributeCallback
			{
			public:
				ObliqueAttributeCallback(ObliqueConverter* owner);
				~ObliqueAttributeCallback();
				void onSetAttirubte(String name, Cesium::FeatureBatchTable* featurebatchtable);
				String _name;
			private:
				ObliqueConverter* _owner;
			};

			void split(osg::Group* node, OC::Cesium::Tile* parentTile, bool shouldSplit, int depth, String destDir, int code);
		private:
			ConvertWorkqueue workqueue;
		};
	}
}

#endif
