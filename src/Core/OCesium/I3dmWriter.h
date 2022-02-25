#ifndef _OC_I3DM_WRITER_H
#define _OC_I3DM_WRITER_H

#include "CesiumDefine.h"
#include "FeatureBatchTable.h"
#include "BaseWriter.h"

namespace OC
{
	namespace Cesium
	{
		class _CesiumExport I3dmWriter : public BaseWriter
		{
		public:
			I3dmWriter();

			void push(osg::Vec3 pos, osg::Vec3 normalU, osg::Vec3 normalR);
			void push_position(osg::Vec3 pos);
			void push_scale(osg::Vec3 scale);
			void push_normalU(osg::Vec3 normal);
			void push_normalR(osg::Vec3 normal);
			void setNode(osg::Node* node) { mNode = node; }
			void setUrl(String url);
			String getUrl();

			size_t size();
			osg::ref_ptr<osg::Node>			mNode;
			osg::ref_ptr<osg::Vec3Array>	scales;
			osg::ref_ptr<osg::Vec3Array>	normalsU;
			osg::ref_ptr<osg::Vec3Array>	normalsR;
			osg::ref_ptr<osg::Vec3Array>	positions;

			GLTFObject* getGltfObject();
			FeatureBatchTable* getFeatureBatchTable();

			virtual bool writeToFile(String filename);
			virtual bool writeToStream(std::ostream& output);
		protected:
			~I3dmWriter();
		private:
			osg::ref_ptr<GLTFObject>		mGltfObject;
			osg::ref_ptr<FeatureBatchTable> mFeatureBatchTable;
			String	mURL;
		};
	}
}

#endif

