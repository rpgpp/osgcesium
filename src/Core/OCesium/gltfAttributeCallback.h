#ifndef _GLTF_ATTRIBUTE_CALLBACK_H__
#define _GLTF_ATTRIBUTE_CALLBACK_H__

#include "glTFOptimize.h"
#include "CesiumTool.h"

namespace OC
{
	namespace Cesium
	{
		class gltfAttributeCallback : public Cesium::FeatureAttributeCallback
		{
		public:
			gltfAttributeCallback();
			~gltfAttributeCallback();

			void readFile(String filename);
			osg::ref_ptr<OCHeaderInfo>		header;
			std::map<String, osg::ref_ptr<TemplateRecord> > mAttributeMap;
			virtual void onSetAttirubte(String name, Cesium::FeatureBatchTable* table);
		};
	}
}

#endif