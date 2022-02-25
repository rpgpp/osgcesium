#include "I3dmWriter.h"
#include "GLTFObject.h"

namespace OC
{

	Cesium::I3dmWriter::I3dmWriter()
		:mFeatureBatchTable(new FeatureBatchTable(FBT_I3DM))
	{
		mGltfObject = new GLTFObject(mFeatureBatchTable);
		positions = new osg::Vec3Array();
	}

	Cesium::I3dmWriter::~I3dmWriter()
	{
	}

	void Cesium::I3dmWriter::setUrl(String url)
	{
		mURL = url;
	}

	String Cesium::I3dmWriter::getUrl()
	{
		return mURL;
	}

	size_t Cesium::I3dmWriter::size()
	{
		return positions->size();
	}

	void Cesium::I3dmWriter::push(osg::Vec3 pos, osg::Vec3 normalU, osg::Vec3 normalsR)
	{
		push_position(pos);
		push_normalU(normalU);
		push_normalR(normalsR);
	}

	void Cesium::I3dmWriter::push_position(osg::Vec3 pos)
	{
		positions->push_back(pos);
	}

	void Cesium::I3dmWriter::push_scale(osg::Vec3 scale)
	{
		if (!scales.valid())
		{
			scales = new osg::Vec3Array();
		}
		scales->push_back(scale);
	}

	void Cesium::I3dmWriter::push_normalU(osg::Vec3 normal)
	{
		if (!normalsU.valid())
		{
			normalsU = new osg::Vec3Array();
		}
		normalsU->push_back(normal);
	}

	void Cesium::I3dmWriter::push_normalR(osg::Vec3 normal)
	{
		if (!normalsR.valid())
		{
			normalsR = new osg::Vec3Array();
		}
		normalsR->push_back(normal);
	}

	bool Cesium::I3dmWriter::writeToFile(String filename)
	{							
		std::fstream fout(filename.c_str(), std::ios::out | std::ios::binary);
		if (!fout.is_open())
			return false;

		writeToStream(fout);

		fout.close();

		return true;
	}

	bool Cesium::I3dmWriter::writeToStream(std::ostream& output)
	{
		uint32 pos_beg = output.tellp();

		mFeatureBatchTable->setFeatureLength(positions->size());
		mFeatureBatchTable->setBatchLength(positions->size());
		mFeatureBatchTable->writeFeatureBody("POSITION", positions.get());
		mFeatureBatchTable->writeFeatureBody("NORMAL_UP", normalsU.get());
		mFeatureBatchTable->writeFeatureBody("NORMAL_RIGHT", normalsR.get());
		mFeatureBatchTable->writeFeatureBody("SCALE_NON_UNIFORM", scales.get());
		mFeatureBatchTable->format();

		//write gltf body
		uint32 headerBytesLength = mFeatureBatchTable->bytesLength();
		output.seekp(pos_beg + headerBytesLength);
		uint32 gltfBytesLength = 0;

		if (mNode.valid())
		{
			mGltfObject->convert(mNode.get());
			mGltfObject->writeToStream(output, true);
			gltfBytesLength = mGltfObject->bytesLength();
			uint32 gltfDataPadding = 4 - (gltfBytesLength % 4);
			if (gltfDataPadding == 4) gltfDataPadding = 0;
			output.write("\0\0\0", gltfDataPadding);
			gltfBytesLength += gltfDataPadding;
		}
		else
		{	
			String url = mURL;
			while(url.length() % 4 != 0)
				url.push_back(' ');
			output << url;
			gltfBytesLength = url.length();
			mFeatureBatchTable->setFormat(0);
		}

		mBytesLength = headerBytesLength + gltfBytesLength;

		//write header
		output.seekp(pos_beg);
		mFeatureBatchTable->setExternalBytesLength(gltfBytesLength);
		mFeatureBatchTable->writeToStream(output);

		output.seekp(0,std::ios::end);

		return mBytesLength != 0;
	}

	Cesium::GLTFObject* Cesium::I3dmWriter::getGltfObject()
	{
		return nullptr;
	}

	Cesium::FeatureBatchTable* Cesium::I3dmWriter::getFeatureBatchTable()
	{
		return mFeatureBatchTable.get();
	}

}
