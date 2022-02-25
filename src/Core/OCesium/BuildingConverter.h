#ifndef _Building_CONVETER_H__
#define _Building_CONVETER_H__

#include "glTFOptimize.h"
#include "CesiumTool.h"
#include <osgEarth/ExtrudeGeometryFilter>

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport BuildingConverter
		{
		public:
			int split(const GeoExtent& extent, OC::Cesium::Tile* parentTile, unsigned depth, String destDir);
			OC::Cesium::Tile* buildTile(FeatureList& list, int depth, String uri);
			BuildingConverter();
			~BuildingConverter();

			void readShp(String filename);
			void output();

			String nameExpression = "Id";
			String heightExpression = "3.5 * [Floor]";
			float constantHeight = 100.0;
			String mErrors;
			String outputPath;
			int splitLevel = 4;
			int mMaxNodeInTile = 1500;
			String resource_lib;
			StringVector	mWriteAttributeList;
			class BuildingAttributeCallback : public Cesium::FeatureAttributeCallback
			{
			public:
				BuildingAttributeCallback(BuildingConverter* owner);
				~BuildingAttributeCallback();

				virtual void onSetAttirubte(String name, Cesium::FeatureBatchTable* table);

			private:
				BuildingConverter* _owner;
			};
		private:
			void initStyleSheet();
			osg::Matrix transform;
			unsigned	maxDepth;
			osg::ref_ptr<FeatureSource>				mFeatureSource;
			osg::BoundingBox						mRootBoudingBox;
			FilterContext							mFilterContext;
			ExtrudeGeometryFilter					extrude;
			std::map<FeatureID, char>				mWritedFidMap;
			std::vector<double>						mGeometricError;
			osg::ref_ptr<BuildingAttributeCallback> mBuildingAttributeCallback;
		};
	}
}
#endif