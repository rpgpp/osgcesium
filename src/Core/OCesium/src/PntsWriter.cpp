#include "PntsWriter.h"
#include "X3DM.h"

namespace OC
{
    Cesium::PntsWriter::PntsWriter()
		:mFeatureBatchTable(FBT_PNTS)
    {
        positions = new osg::Vec3Array();
    }

    Cesium::PntsWriter::~PntsWriter()
    {
    }

    void Cesium::PntsWriter::push(osg::Vec3 pos)
    {
        positions->push_back(pos);
    }

	void Cesium::PntsWriter::push_normal(osg::Vec3 normal)
	{
		if (!normals.valid())
		{
			normals = new osg::Vec3Array;
		}
		normals->push_back(normal);
	}

	void Cesium::PntsWriter::push_color(osg::Vec3ub color)
	{
		if (!colors.valid())
		{
			colors = new osg::Vec3ubArray;
		}
		colors->push_back(color);
	}

	void Cesium::PntsWriter::push_color(uint8 r, uint8 g, uint8 b)
	{
		push_color(osg::Vec3ub(r,g,b));
	}

	void Cesium::PntsWriter::push_positionAndBatchId(osg::Vec3 pos, int batchId)
	{
		push(pos);
		if (!batchIdArray.valid())
		{
			batchIdArray = new osg::UIntArray;
		}
		if (batch_length == batchId)
		{
			batch_length++;
		}
		batchIdArray->push_back(batchId);
	}

	bool Cesium::PntsWriter::writeToFile(String filename)
	{
		std::fstream fout(filename.c_str(), std::ios::out | std::ios::binary);
		if (!fout.is_open())
			return false;

		return writeToStream(fout);
	}

	bool Cesium::PntsWriter::writeToStream(std::ostream& output)
	{
		mFeatureBatchTable.setFeatureLength(positions->size());
		mFeatureBatchTable.setBatchLength(batch_length);

		mFeatureBatchTable.writeFeatureBody("POSITION", positions.get());
		mFeatureBatchTable.writeFeatureBody("NORMAL", normals.get());
		mFeatureBatchTable.writeFeatureBody("BATCH_ID", batchIdArray.get());
		mFeatureBatchTable.writeFeatureBody("RGB", colors.get());

		mFeatureBatchTable.format();
		mFeatureBatchTable.writeToStream(output);

		return true;
	}
	
}

