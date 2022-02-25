#include "SaveStrategy.h"
#include "OCesium/B3dmWriter.h"
#include "OCesium/TDTiles.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport TilesetSaveStrategy :public SaveStrategy
		{
		public:
			TilesetSaveStrategy();
			~TilesetSaveStrategy();
			virtual bool	init();
			virtual bool	writeFeature(FeatureRecord* record);

		protected:
			typedef std::map<String, osg::ref_ptr<Cesium::I3dmWriter> > GlobalTiles;
			void							writeTileset();
			GlobalTiles						findGlobal(IGroupModelData* groupMD);
		private:
			osg::BoundingBox											mRootBoundBox;
			osg::ref_ptr<TDTILES::Tileset>								mRootTileset;
			std::map<String,osg::ref_ptr<Cesium::B3dmWriter> >			mTiltWriterMap;
		};
	}
}


