#include "X3DM.h"

namespace OC
{
	namespace Cesium
	{
		bool X3DMWriter::writePNTS(const String& location,
			const String& featureTableJSON,
			const String& featureTableBody,
			const String& batchTableJSON,
			const String& batchTableBody)
		{
			std::fstream fout(location.c_str(), std::ios::out | std::ios::binary);
			if (!fout.is_open())
				return false;

			int featureTablePadding = 4 - (featureTableJSON.length() % 4);
			if (featureTablePadding == 4) featureTablePadding = 0;

			int batchTablePadding = 4 - (batchTableJSON.length() % 4);
			if (batchTablePadding == 4) batchTablePadding = 0;

			// assemble the header:
			b3dmheader header;
			header.magic[0] = 'p', header.magic[1] = 'n', header.magic[2] = 't', header.magic[3] = 's';
			header.version = 1;
			header.featureTableJSONByteLength = featureTableJSON.length() + featureTablePadding;
			header.featureTableBinaryByteLength = featureTableBody.length();
			header.batchTableJSONByteLength = batchTableJSON.length() + batchTablePadding;
			header.batchTableBinaryByteLength = 0;
			header.byteLength =
				sizeof(b3dmheader) +
				header.featureTableJSONByteLength +
				header.featureTableBinaryByteLength +
				header.batchTableJSONByteLength +
				header.batchTableBinaryByteLength;

			fout.write((const char*)&header, sizeof(b3dmheader));
			fout.write(featureTableJSON.c_str(), featureTableJSON.length());
			fout.write("   ", featureTablePadding);
			fout.write(featureTableBody.c_str(), featureTableBody.length());
			fout.write(batchTableJSON.c_str(), batchTableJSON.length());
			fout.write("\0\0\0", batchTablePadding);

			fout.close();

			return true;
		}
	}
}