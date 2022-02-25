#ifndef _GLTF_OPTIMIZE_H__
#define _GLTF_OPTIMIZE_H__

#include "CesiumDefine.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport glTFVisitor : public osg::NodeVisitor
		{
		public:
			glTFVisitor();
			~glTFVisitor();

			void apply(osg::StateSet& stateset);
			void apply(osg::Geode& geode);
			void apply(osg::Node& node);

			bool							removeStateset;
			osgEarth::optional<osg::Vec3d>	offset;
		};
	}
}


#endif
