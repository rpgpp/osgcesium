#include "NCArchive.h"
#include "OCNetCDF/NetCDFReader.h"
#include "OCMain/DataStream.h"
#include "OCZip/zip.h"
#include "OCesium/CesiumTool.h"

NcArchive::NcArchive(void)
{
	supportsExtension("nc", "OC nc data format");
	supportsExtension("grib", "OC GRB2 data format");
	supportsExtension("grb2", "OC GRB2 data format");
	//supportsExtension("bin", "cin data format");
	//supportsExtension("bz2", "swan zip data format");
	//supportsExtension("awx", "awx data format");
	supportsExtension("npy", "npy data format");
}

NcArchive::~NcArchive(void)
{

}

osgDB::ReaderWriter::ReadResult NcArchive::readNode(std::istream& fin, const Options* options) const
{
	return osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED;
}

osgDB::ReaderWriter::ReadResult NcArchive::readNode(const String& fileName, const osgDB::ReaderWriter::Options* options) const
{
	String ext = osgDB::getLowerCaseFileExtension(fileName);
	if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

	return osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED;
}

osgDB::ReaderWriter::ReadResult NcArchive::readImage(const String& fileName, const osgDB::ReaderWriter::Options* options) const
{
	String ext = osgDB::getLowerCaseFileExtension(fileName);
	if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

	if (options == NULL)
	{
		return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;
	}

	String varname = options->getPluginStringData("varname");

	if (!FileUtil::FileExist(fileName) && varname != "60443D5C")
	{
		return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;
	}


	CNetCDFReader reader;
	osg::ref_ptr<osg::Image> image;

	if (ext == "bin" || ext == "bz2")
	{
		image = reader.readSW(fileName, varname);
	}
	else if (ext == "grb2" || ext == "grib")
	{
		image = reader.readGRB2(fileName, varname);
	}
	else if (ext == "awx")
	{
		image = reader.readAWX(fileName, varname);
	}
	else if (ext == "npy")
	{
		image = reader.readNpy(fileName, varname);
	}
	else
	{
		image = reader.readNC(fileName, varname);
	}

	return image.get();
}

osgDB::ReaderWriter::ReadResult NcArchive::readObject(const String& fileName, const osgDB::ReaderWriter::Options* options) const
{
	String ext = osgDB::getLowerCaseFileExtension(fileName);
	if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;
	if (options == NULL)
	{
		return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;
	}

	String varname = options->getPluginStringData("varname");

	CNetCDFReader reader;
	return reader.readVolume(fileName, varname);
}

inline String getJSON(const osg::Image& image)
{
	String positionStr, meterStr, usoStr, extentStr, internalFormatStr;
	image.getUserValue(String("position"), positionStr);
    image.getUserValue(String("meter"), meterStr);
	image.getUserValue(String("u_s_o"), usoStr);
	image.getUserValue(String("extent"), extentStr);
	image.getUserValue(String("internalFormat"), internalFormatStr);
	CVector3 position = StringConverter::parseVector3(positionStr);
    CVector3 meter = StringConverter::parseVector3(meterStr);
	CVector3 u_s_o = StringConverter::parseVector3(usoStr);
	CRectangle extent = StringConverter::parseRectangle(extentStr);
	CVector2 mininum = extent.getMinimum();
	CVector2 maxinum = extent.getMaximum();

	StringStream sstream;
	sstream << "{\"position\":{\"x\":" << position.x << ",\"y\":" << position.y << ",\"z\":" << position.z << "}"
		<< ",\"meter\":{\"width\":" << meter.x << ",\"height\":" << meter.y << ",\"depth\":" << meter.z << "}"
		<< ",\"source\":{\"width\":" << image.s() << ",\"height\":" << image.t() << ",\"depth\":" << image.r() << "}"
		<< ",\"u_s_o\":{\"scaler\":" << u_s_o.x << ",\"clampMin\":" << u_s_o.y << ",\"clampMax\":" << u_s_o.z << "}"
		<< ",\"extent\":{\"xmin\":" << mininum.x << ",ymin:" << mininum.y << ",\"xmax\":" << maxinum.x << ",\"ymax\":" << maxinum.y << "}"
		<< ",\"internalFormat\":" << internalFormatStr << "}";

	return sstream.str();
}

osgDB::ReaderWriter::WriteResult NcArchive::writeImage(const osg::Image& image, const String& fileName, const osgDB::ReaderWriter::Options* options) const
{
	String ext = osgDB::getLowerCaseFileExtension(fileName);
	if (options == NULL || !acceptsExtension(ext)) return osgDB::ReaderWriter::WriteResult::FILE_NOT_HANDLED;

	ext = options->getOptionString();

	String writefile = StringUtil::replaceAll(fileName, ".nc", "." + ext);
	writefile = StringUtil::replaceAll(writefile, ".bz2", "." + ext);
	writefile = StringUtil::replaceAll(writefile, ".GRB2", "." + ext);
	writefile = StringUtil::replaceAll(writefile, ".grib", "." + ext);
	writefile = StringUtil::replaceAll(writefile, ".AWX", "." + ext);
	writefile = StringUtil::replaceAll(writefile, ".npy", "." + ext);
	if (StringUtil::endsWith(writefile, ".bin"))
	{
		writefile = StringUtil::replaceAll(writefile, ".bin", "." + ext);
	}

	if (ext == "zip")
	{
		String jsonStr = getJSON(image);

		int ps = osg::Image::computeNumComponents(image.getPixelFormat());
		//
		size_t size = image.getTotalSizeInBytes();
		MemoryDataStreamPtr imageStream(new MemoryDataStream(size));
		for (int r = 0; r < image.r(); ++r)
		{
			for (int t = 0; t < image.t(); ++t)
			{
				const char* data = (const char*)image.data(0, t, r);
				imageStream->write((const char*)data, image.s() * ps);
			}
		}

		uLong srcLen = imageStream->size();

		if (srcLen > 0)
		{
			if (zipFile f = zipOpen(writefile.c_str(), APPEND_STATUS_CREATE))
			{
				String zipObject = osgDB::getNameLessExtension(osgDB::getSimpleFileName(writefile));
				if (ZIP_OK == zipOpenNewFileInZip(f, (zipObject + ".raw").c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
				{
					if (ZIP_OK == zipWriteInFileInZip(f, imageStream->getPtr(), srcLen))
					{
						zipCloseFileInZip(f);
					}
				}
				if (ZIP_OK == zipOpenNewFileInZip(f, (zipObject + ".json").c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
				{
					if (ZIP_OK == zipWriteInFileInZip(f, jsonStr.c_str(), jsonStr.length()))
					{
						zipCloseFileInZip(f);
					}
				}
				zipClose(f, 0);
				return osgDB::ReaderWriter::WriteResult::FILE_SAVED;
			}
		}
	}
	else if (ext == "json")
	{
		String jsonStr = getJSON(image);
		std::ofstream ofs(writefile.c_str());
		ofs << jsonStr;
		ofs.close();
	}
	else if(ext == "raw")
	{
		std::ofstream ofs(writefile.c_str(), std::fstream::binary);
		for (int r = 0; r < image.r(); ++r)
		{
			for (int t = 0; t < image.t(); ++t)
			{
				const char* data = (const char*)image.data(0, t, r);
				ofs.write((const char*)data, image.s() * 4);
			}
		}
		ofs.close();
		return osgDB::ReaderWriter::WriteResult::FILE_SAVED;
	}
	else if (ext == "w3dm")
	{
		Cesium::CesiumTool::writeW3DM(image, writefile);
		return osgDB::ReaderWriter::WriteResult::FILE_SAVED;
	}

	return osgDB::ReaderWriter::WriteResult::FILE_NOT_HANDLED;
}

osgDB::ReaderWriter::WriteResult NcArchive::writeImage(const osg::Image& image, std::ostream& stream, const osgDB::ReaderWriter::Options* options) const
{
	if (options == NULL) return osgDB::ReaderWriter::WriteResult::FILE_NOT_HANDLED;

	String fileName;
	String ext;

	std::istringstream iss(options->getOptionString());
	iss >> ext;
	iss >> fileName;

	String writefile = StringUtil::replaceAll(fileName, ".nc", "." + ext);
	writefile = StringUtil::replaceAll(writefile, ".bz2", "." + ext);

	if (ext == "zip")
	{
		String jsonStr = getJSON(image);

		int ps = osg::Image::computeNumComponents(image.getPixelFormat());
		//
		size_t size = image.getTotalSizeInBytes();
		MemoryDataStreamPtr imageStream(new MemoryDataStream(size));
		for (int r = 0; r < image.r(); ++r)
		{
			for (int t = 0; t < image.t(); ++t)
			{
				const char* data = (const char*)image.data(0, t, r);
				imageStream->write((const char*)data, image.s() * ps);
			}
		}

		uLong srcLen = imageStream->size();
		uLong destLen = srcLen;

		if (srcLen > 0)
		{
			MemoryDataStreamPtr destStream(new MemoryDataStream(destLen));
			int err = compress(destStream->getPtr(), &destLen, imageStream->getPtr(), srcLen);
			if (err == ZIP_OK)
			{
				if (zipFile f = zipOpenStd("", APPEND_STATUS_CREATE,&stream))
				{
					String zipObject = osgDB::getNameLessExtension(osgDB::getSimpleFileName(writefile));
					if (ZIP_OK == zipOpenNewFileInZip(f, (zipObject + ".raw").c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
					{
						if (ZIP_OK == zipWriteInFileInZip(f, imageStream->getPtr(), srcLen))
						{
							zipCloseFileInZip(f);
						}
					}
					if (ZIP_OK == zipOpenNewFileInZip(f, (zipObject + ".json").c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
					{
						if (ZIP_OK == zipWriteInFileInZip(f, jsonStr.c_str(), jsonStr.length()))
						{
							zipCloseFileInZip(f);
						}
					}
					zipClose(f, 0);
					return osgDB::ReaderWriter::WriteResult::FILE_SAVED;
				}
			}
		}
	}
	else if (ext == "json")
	{
		
	}
	else if (ext == "raw")
	{

	}
	else if (ext == "w3dm")
	{
		Cesium::CesiumTool::writeW3DM(image , stream);
		return osgDB::ReaderWriter::WriteResult::FILE_SAVED;
	}

	return osgDB::ReaderWriter::WriteResult::FILE_NOT_HANDLED;
}

REGISTER_OSGPLUGIN(nc, NcArchive)
