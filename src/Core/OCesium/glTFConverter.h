#ifndef _GLTF_CONVETER_H__
#define _GLTF_CONVETER_H__

#include "gltfAttributeCallback.h"
#include "MapProject.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport glTFConverter : public osg::Referenced
		{
		public:
			glTFConverter();
			~glTFConverter();

			void readAttributeMap(String filename);
			void readPrjFile(String filename);
			void setLocaltion(CVector3 location);
			void output();
			void writeToGltf();
			void writeToGlb();
			bool readNodeFile(String filename, osgDB::Options* options = NULL);
			void optimize(osg::Node* node);
			void resetNode();
			CVector3									mPrjOffset;
			osgEarth::optional<String>					mPrjCS;
			bool localOffset = false;
			bool upAxisZ = true;
			bool rotate = false;
			String outputPath;
		private:
			void init();
			String										mNodeFilename;
			CVector3									mLocation;
			osg::ref_ptr<osg::Node>						mNode;
			osg::ref_ptr<MapProject>					mProjectTool;
			osg::ref_ptr<gltfAttributeCallback>			mAttributeCallbak;
		};
	}
}


#endif
