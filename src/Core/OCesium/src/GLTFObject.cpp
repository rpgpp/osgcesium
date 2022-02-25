#include "GLTFObject.h"

namespace OC
{
	namespace Cesium
	{
		GLTFObject::GLTFObject(FeatureBatchTable* featureBatchTable)
			:BatchGroup(featureBatchTable)
		{
		}

		GLTFObject::~GLTFObject()
		{
		}

		bool GLTFObject::writeToFile(String filename, bool isBinary)
		{
			std::fstream fout(filename.c_str(), std::ios::out | std::ios::binary);
			if (!fout.is_open())
				return false;

			bool ret = writeToStream(fout, isBinary);

			fout.close();

			return ret;
		}

		bool GLTFObject::writeToStream(std::ostream& output, bool isBinary)
		{
			submitBatch();

			size_t pos0 = output.tellp();
			if (_gltfHelper.writeGltfSceneToStream(output, isBinary))
			{
				size_t pos1 = output.tellp();
				mBytesLength = pos1 - pos0;
			}

			return mBytesLength != 0;
		}

		uint32 GLTFObject::bytesLength()
		{
			return mBytesLength;
		}
	}
}