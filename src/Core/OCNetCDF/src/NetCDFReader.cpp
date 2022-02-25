#include "NetCDFReader.h"
#include "NcZQ.h"
#include "NcZT.h"
#include "NcUV.h"
#include "NcDBZ.h"
#include "OCMain/JsonPackage.h"
#include "npy/npyTemprature.h"
#include "NcepWind.h"

namespace OC
{
	using namespace Volume;
	CNetCDFReader::CNetCDFReader()
	{

	}

	CNetCDFReader::~CNetCDFReader()
	{

	}

	osg::Image* CNetCDFReader::readNpy(String filename, String var)
	{
		return NULL;
	}

	osg::Image* CNetCDFReader::readAWX(String filename, String var)
	{
		return NULL;
	}

	osg::Image* CNetCDFReader::readGRB2(String filename, String var_name)
	{
		//String jsonStr = Singleton(ConfigManager).getStringValue(var_name);
		//Json::Reader reader;
		//Json::Value root;
		//if (reader.parse(jsonStr, root))
		//{
		//	String type = JsonPackage::getString(root, "type");
		//	if (type == "uv")
		//	{
		//		CUVComponent volume;
		//		volume.nameU = JsonPackage::getString(root, "u");
		//		volume.nameV = JsonPackage::getString(root, "v");
		//		volume.levelX = JsonPackage::getString(root, "x");
		//		volume.levelY = JsonPackage::getString(root, "y");
		//		volume.levelZ = JsonPackage::getString(root, "z");
		//		volume.interval = JsonPackage::getDouble(root, "interval");
		//		return volume.readGrb2(filename, var_name);
		//	}
		//	else if (type == "necpuv")
		//	{
		//		
		//	}
		//}

		return NULL;
	}

	osg::Image* CNetCDFReader::readNC(String filename, String var_name)
	{
		if (var_name == "z_q")
		{

		}
		else if (var_name == "z_t")
		{

		}
		else if (var_name == "u_v")
		{
			//CUVComponent volume;
			//if (volume.open(filename))
			//{
			//	return volume.readImage(filename, var_name);
			//}
		}
		else
		{
			//String jsonStr = Singleton(ConfigManager).getStringValue(var_name);
			//Json::Reader reader;
			//Json::Value root;
			//if (reader.parse(jsonStr, root))
			//{
			//	String type = JsonPackage::getString(root, "type");
			//	if (type == "uv")
			//	{
			//		CUVComponent volume;
			//		volume.nameU = JsonPackage::getString(root, "u");
			//		volume.nameV = JsonPackage::getString(root, "v");
			//		volume.levelX = JsonPackage::getString(root, "x");
			//		volume.levelY = JsonPackage::getString(root, "y");
			//		volume.levelZ = JsonPackage::getString(root, "z");
			//		volume.interval = JsonPackage::getDouble(root, "interval");
			//		if (volume.open(filename))
			//		{
			//			return volume.readImage(filename, var_name);
			//		}
			//	}
			//	else if (type == "fy2")
			//	{
			//		return readAWX(filename, var_name);
			//	}
			//	else if (type == "npy")
			//	{
			//		return readNpy(filename, var_name);
			//	}
			//}

		}
		return NULL;
	}

	Volume::CVolumeObject* CNetCDFReader::readVolume(String filename, String var)
	{
		String name, ext, path;
		StringUtil::splitFullFilename(filename, name, ext, path);
		StringUtil::toLowerCase(ext);
		if (ext == "grib")
		{
			String jsonStr = Singleton(ConfigManager).getStringValue(var);
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(jsonStr, root))
			{
				String type = JsonPackage::getString(root, "type");
				if (type == "necpuv")
				{
					osg::ref_ptr<NcepWind> volume = new NcepWind;
					volume->nameU = JsonPackage::getString(root, "u");
					volume->nameV = JsonPackage::getString(root, "v");
					volume->levelX = JsonPackage::getString(root, "x");
					volume->levelY = JsonPackage::getString(root, "y");
					volume->levelZ = JsonPackage::getString(root, "z");
					volume->interval = JsonPackage::getDouble(root, "interval");
					volume->flipX = JsonPackage::getDouble(root, "flipX");
					volume->convert(filename);
					return volume.release();
				}
			}
		}
		else if (ext == "npy")
		{
			osg::ref_ptr<npyTemprature> volume = new npyTemprature(var);
			volume->convert(filename);
			return volume.release();
		}
		else if (ext == "nc")
		{
			if (var == "DBZ")
			{
				osg::ref_ptr<NcDBZ> volume = new NcDBZ(var);
				volume->convert(filename);
				return volume.release();
			}
			else if (var == "z_q")
			{
				osg::ref_ptr<NcZQ> volume = new NcZQ(var);
				volume->convert(filename);
				return volume.release();
			}
			else if (var == "z_t")
			{
				osg::ref_ptr<NcZT> volume = new NcZT(var);
				volume->convert(filename);
				return volume.release();
			}
			else
			{
				String jsonStr = Singleton(ConfigManager).getStringValue(var);
				Json::Reader reader;
				Json::Value root;
				if (reader.parse(jsonStr, root))
				{
					String type = JsonPackage::getString(root, "type");
					if (type == "uv")
					{
						osg::ref_ptr<NcUV> volume = new NcUV();
						volume->nameU = JsonPackage::getString(root, "u");
						volume->nameV = JsonPackage::getString(root, "v");
						volume->levelX = JsonPackage::getString(root, "x");
						volume->levelY = JsonPackage::getString(root, "y");
						volume->levelZ = JsonPackage::getString(root, "z");
						volume->interval = JsonPackage::getDouble(root, "interval");
						volume->convert(filename);
						return volume.release();
					}
				}
			}
		}
		 
		return NULL;
	}

	osg::Image* CNetCDFReader::readSW(String filename, String var_name)
	{
		//if (var_name == "SA" || var_name == "SB"
		//	|| var_name == "CA")
		//{
		//	CINRad volume(filename);
		//	return volume.readImage(filename, var_name);
		//}
		//else if (var_name == "CD25FDE5")
		//{
		//	CINRad volume(filename);
		//	volume.setType(SASBCAXFMT);
		//	return volume.readImage(filename, var_name);
		//}
		//else if (var_name == "CD25FDE6")
		//{
		//	CINRad volume(filename);
		//	volume.setType(SASBCAXFMT);
		//	return volume.readImageForVolume(filename, var_name);
		//}
		//else if (var_name == "EF6A0A74")
		//{
		//	CINRad volume(filename);
		//	return volume.readImageForVolume(filename, var_name);
		//}
		//else
		//{
		//	SWAN volume(filename);
		//	return volume.readImage(filename, var_name);
		//}

		return NULL;
	}
}