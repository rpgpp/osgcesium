#include "CmptWriter.h"

namespace OC
{namespace Cesium{

	CmptWriter::CmptWriter()
	{

	}

	CmptWriter::~CmptWriter()
	{

	}

	void CmptWriter::push(BaseWriter* object)
	{
		mTiles.push_back(object);
	}

	void CmptWriter::writeCmpt(String filename)
	{
		std::fstream fout(filename.c_str(), std::ios::out | std::ios::binary);
		if (!fout.is_open())
			return;

		cmptheader header;
		header.magic[0] = 'c', header.magic[1] = 'm', header.magic[2] = 'p', header.magic[3] = 't';
		header.version = 1;
		header.tilesLength = getLength();

		uint32 byteslength = sizeof(cmptheader);
		fout.seekp(byteslength);

		for (auto i : mTiles)
		{
			i->writeToStream(fout);
			byteslength += i->bytesLength();
		}

		header.byteLength = byteslength;
		fout.seekp(std::ios::beg);
		fout.write((const char*)&header, sizeof(cmptheader));

		fout.close();
	}

	uint32 CmptWriter::getLength()
	{
		return (uint32)mTiles.size();
	}

}}//namespace