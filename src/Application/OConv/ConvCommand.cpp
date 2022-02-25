#include "ConvCommand.h"
#include "RadarStationKML.h"
#include "OCesium/CesiumTool.h"
#include "OCLayer/ModelUtil.h"
#include "OCesium/KtxCompress.h"
#include "OCesium/WebpEncoder.h"
#include "OCLayer/ReadFileCallback.h"
#include "OCZip/PakoZip.h"
#include "OCVolume/Volume.h"
#include <osgEarth/FeatureSource>
#include <osgEarth/OGRFeatureSource>
#include "Geography/ShpDrawer.h"
#include "Geography/ShpDrawerHelper.h"
#include "Geography/Kriging.h"

using namespace OC::Volume;

void initEnviroment()
{
    HMODULE hModule = ::GetModuleHandle(NULL);
    char filename[MAX_PATH + 1];
    String strRtn = "";
    if (GetModuleFileNameA((HMODULE)hModule, filename, MAX_PATH))
    {
        strRtn = filename;
    }
    String path = osgDB::getFilePath(filename);
	Environment* mEnvironment = new Environment(0x6C6777);
    mEnvironment->setAppDir(path);
    new ConfigManager(Singleton(Environment).getAppDir() + "config" + "/config.ini");
	Singleton(LogManager).createLog("", true, true);
}

void conv2obj(OC::Json::Value& root)
{
    String filename = JsonPackage::getString(root, "filename");
    osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename);
    if (node.valid())
    {
        osgUtil::Optimizer optimizer;
        optimizer.optimize(node, osgUtil::Optimizer::MAKE_FAST_GEOMETRY
            | osgUtil::Optimizer::SHARE_DUPLICATE_STATE
            | osgUtil::Optimizer::TRISTRIP_GEOMETRY
            | osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS);

        String basename, extension, out_dir;
        StringUtil::splitFullFilename(filename, basename, extension, out_dir);
        String out_file = out_dir + basename + "_conv2obj.obj";
        osgDB::writeNodeFile(*node, out_file);
    }
}

void offset2origin(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
	StringStream sstream;

	options->setOptionString(sstream.str());
	String offsetStr = JsonPackage::getString(root, "offset");
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename, options);
	if (node.valid())
	{
		OC::Cesium::glTFVisitor gVisitor;
		long offsetX = 0;
		long offsetY = 0;
		long offsetZ = 0;
		if (offsetStr.empty())
		{
			offsetX = node->computeBound().center().x();
			offsetY = node->computeBound().center().y();
			offsetZ = node->computeBound().center().z();
		}
		else
		{
			CVector3 offset = StringConverter::parseVector3(offsetStr);
			offsetX = offset.x;
			offsetY = offset.y;
			offsetZ = offset.z;
		}
		gVisitor.offset = osg::Vec3d(offsetX, offsetY, offsetZ);
		gVisitor.traverse(*node);
		String basename, extension, out_dir;
		StringUtil::splitFullFilename(filename, basename, extension, out_dir);
		String lastFix = "_" + StringConverter::toString(offsetX) + "_" + StringConverter::toString(offsetY) + "_" + StringConverter::toString(offsetZ);
		String out_file = out_dir + basename + lastFix + "." + extension;
		//osgUtil::SmoothingVisitor sv;
		//sv.traverse(*node);
		osgDB::writeNodeFile(*node, out_file, options);
	}
}

void npy2txt(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");
	osgDB::readImageFile(filename);
}

void Packmetadata(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");

	if (FileUtil::FileExist(filename))
	{
		osg::ref_ptr<osg::Group> group = new osg::Group;
		String file = osgDB::getSimpleFileName(filename);
		String directory = osgDB::convertFileNameToUnixStyle(StringUtil::replaceAll(filename, file, "data/"));
		osgDB::DirectoryContents contents = osgDB::getDirectoryContents(directory);
		osgDB::DirectoryContents::iterator it = contents.begin();
		for (; it != contents.end(); it++)
		{
			String subPath = *it;
			if (subPath == ".." || subPath == ".")
			{
				continue;
			}
			else
			{
				if (osgDB::DIRECTORY == osgDB::fileType(directory + "/" + subPath))
				{
					String featureUrl = subPath + "/" + subPath + ".osgb";
					String file = directory + featureUrl;
					if (FileUtil::FileExist(file))
					{
						osg::ProxyNode* node = new osg::ProxyNode;
						node->setFileName(0, file);
						node->setLoadingExternalReferenceMode(osg::ProxyNode::LOAD_IMMEDIATELY);
						group->addChild(node);
					}
				}
			}
		}
		if (group->getNumChildren() > 0)
		{
			osgDB::writeNodeFile(*group, StringUtil::replaceAll(filename, file, "metadata.osg"));
		}
	}
}

void PackJson(OC::Json::Value& root)
{
	String dir = JsonPackage::getString(root, "dir");
	if (dir.empty())
	{
		return;
	}

	String name, ext, path;
	StringUtil::splitFullFilename(dir, name, ext, path);
	String filename = name + ".json";

	StringVector files;
	osgDB::DirectoryContents contents = osgDB::getDirectoryContents(dir);
	osgDB::ofstream ofs((path + filename).c_str());
	ofs << "[";

	size_t size = contents.size();
	for (size_t i = 0; i < size; i++)
	{
		String filepath = contents[i];
		if (filepath == ".." || filepath == ".")
		{
			continue;
		}
		ofs << "\"" << osgDB::getNameLessExtension(osgDB::getSimpleFileName(filepath)) << "\"";
		if(i != size - 1) ofs << ",";
	}
	ofs << "]";
	ofs.close();
}

void PackGroup(OC::Json::Value& root)
{
	StringVector files;
	FileUtil::FindModelFiles("H:/OBJ/", files);
	osg::ref_ptr<osg::Group> g = new osg::Group;
	for (auto f : files)
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(f);
		if (node.valid())
		{
			g->addChild(node);
		}
	}
	osgDB::writeNodeFile(*g, "H:/OBJ/1.osgb");
}

void OCZip(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");

	StringStream zipStream;

	zipFile f = NULL;
	//f = zipOpen(filename.c_str(), APPEND_STATUS_CREATE);
	f = zipOpenStd(filename.c_str(), APPEND_STATUS_CREATE, &zipStream);
	if (f)
	{
		StringStream sstream;
		sstream << "{position:100,200,300}";
		MemoryDataStreamPtr dataStream(new MemoryDataStream(&sstream, false, true));

		if (ZIP_OK == zipOpenNewFileInZip(f, "1.json", NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
		{
			if (ZIP_OK == zipWriteInFileInZip(f, dataStream->getPtr(), dataStream->size()))
			{
				zipCloseFileInZip(f);
			}
		}

		if (ZIP_OK == zipOpenNewFileInZip(f, "2.json", NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
		{
			if (ZIP_OK == zipWriteInFileInZip(f, dataStream->getPtr(), dataStream->size()))
			{
				zipCloseFileInZip(f);
			}
		}

		zipClose(f, 0);

		osgDB::ofstream ofs(filename.c_str());
		MemoryDataStreamPtr dataStream2(new MemoryDataStream(&zipStream, false, true));
		ofs.write((const char*)dataStream2->getPtr(), dataStream2->size());
		ofs.close();
	}
}

void toVolume(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");
	String varname = JsonPackage::getString(root, "varname");
	String destExt = JsonPackage::getString(root, "ext","w3dm");
	osg::ref_ptr<CVolumeObject> vo = Volume::CVolume::convert(filename, varname);
	if (vo.valid())
	{
		String name, ext, path;
		StringUtil::splitFullFilename(filename, name, ext, path);
		String output = JsonPackage::getString(root, "output");
		if (output.empty())
		{
			output = path;
		}
		osgDB::convertFileNameToUnixStyle(output);
		if (!StringUtil::endsWith(output, "/", false))
		{
			output += "/";
		}
		String outfilename = output + name + "." + vo->getVarName() + "." + destExt;
		if (destExt == "zip")
		{
			vo->writeToZipfile(outfilename);
		}
		else
		{
			vo->writeToW3DM(outfilename);
		}
	}
}

void nc2raw(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");

	String name, ext, path;
	StringUtil::splitFullFilename(filename, name, ext, path);

	String varname = JsonPackage::getString(root, "varname");
	String output = JsonPackage::getString(root, "output");
	if (output.empty())
	{
		output = path;
	}
	if (osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("nc"))
	{
		osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
		options->setPluginStringData("varname", varname);
		osg::ref_ptr<osg::Image> image = rw->readImage(filename, options).takeImage();
		if (image.valid())
		{
			int version = JsonPackage::getInt(root, "version", 2);
			String destExt = version == 2 ? "w3dm" : "zip";
			String outfilename = output + name + "." + varname + "." + destExt;
			osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
			options->setOptionString(destExt);
			osgDB::writeImageFile(*image, outfilename, options);
		}
	}
}

inline time_t parseTime(String timeStr)
{
	StringVector sv = StringUtil::split(timeStr," ");
	if (sv.size() == 2)
	{
		StringVector date = StringUtil::split(sv[0], "/");
		StringVector time = StringUtil::split(sv[1], ":");

		struct tm ti;
		memset(&ti, 0, sizeof(ti));

		ti.tm_year = StringConverter::parseInt(date[2]) - 1900;
		ti.tm_mon = StringConverter::parseInt(date[1]) - 1;
		ti.tm_mday = StringConverter::parseInt(date[0]);
		ti.tm_hour = StringConverter::parseInt(time[0]);
		ti.tm_min = StringConverter::parseInt(time[1]);
		ti.tm_sec = StringConverter::parseInt(time[2]);

		return mktime(&ti);
	}

	return 0;
}

void csv23dtiles(OC::Json::Value& root)
{
	String timeStr = "14/5/2021 00:00:33";
	uint32 t = parseTime(timeStr);

	Cesium::PntsWriter writer;

	String filename = JsonPackage::getString(root, "filename");
	String name, ext, path;
	StringUtil::splitFullFilename(filename, name, ext, path);
	String pntsFile = path + name + ".pnts";

	osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	osg::DrawElements* indice = new osg::DrawElementsUInt(GL_LINES);
	osg::ref_ptr<osg::Geode> group = new osg::Geode;
	{
		osg::Geometry* geom = new osg::Geometry;
		group->addDrawable(geom);
		geom->setVertexArray(vertex);
		geom->setNormalArray(normals);
		geom->addPrimitiveSet(indice);
		ModelUtil::setPipeClrMat(group,ColourValue::White);
	}

	Vector4List locations;
	CRectangle rectangle;

	auto getLocation = [&locations, &rectangle, &writer](String filename) {
		osgDB::ifstream ifs(filename.c_str());
		if (ifs)
		{
			String line;
			while (getline(ifs, line))
			{
				StringVector sv = StringUtil::split(line, ",");
				if (sv.size() < 3)
				{
					break;
				}
				String latStr = sv[0];
				String lonStr = sv[1];
				String heightStr = sv[10];
				String timeStr = sv[8];

				CVector4 location;
				location.x = StringConverter::parseReal(lonStr);
				location.y = StringConverter::parseReal(latStr);
				location.z = StringConverter::parseReal(heightStr) * 1000.0;
				location.w = StringConverter::parseReal(sv[2]);
				rectangle.merge(CVector2(location.x, location.y));
				locations.push_back(location);

				uint32 t = parseTime(timeStr);
				//writer.mFeatureBatchTable.pushAttribute("timestamp", t);
				writer.mFeatureBatchTable.pushAttribute("intens", (float)location.w);
			}
		}

		ifs.close();
		return true;
	};

	getLocation(filename);
	for(int i=1;i<4;i++)
	{
		getLocation(path + name + "_" + StringConverter::toString(i) + ".csv");
	}

	std::cout<< "thunder count:" << locations.size() <<std::endl;

	CVector3 worldPosition = ProjectTool::toWorld(rectangle.getCenter());

	uint32 i = 0;
	uint32 batchId = 0;
	for (const auto& location : locations)
	{
		CVector3 position = ProjectTool::toWorld(CVector3(location.x, location.y, location.z));
		CVector3 position0 = position - worldPosition;
		writer.push_positionAndBatchId(Vector3ToVec3(position0), batchId++);
		osg::Vec3 normal = Vector3ToVec3(position);
		writer.push_normal(normal);

		if(location.z > 0.0)
		{
			CVector3 position1 = ProjectTool::toWorld(CVector2(location.x, location.y)) - worldPosition;
			vertex->push_back(Vector3ToVec3(position0));
			vertex->push_back(Vector3ToVec3(position1));
			indice->addElement(i++);
			indice->addElement(i++);
			normals->push_back(normal);
			normals->push_back(normal);
		}

		writer.mFeatureBatchTable.pushAttribute("height", (float)location.z);
		//writer.mFeatureBatchTable.pushAttribute("id", location.z > 0 ? "IC" :"CG");
	}

	writer.writeToFile(pntsFile);
	osg::ref_ptr<TDTILES::Tileset> tileset = OC_NEW_Tiltset();
	tileset->root()->transform() = osg::Matrix::translate(Vector3ToVec3(worldPosition));
	tileset->geometricError() = tileset->root()->geometricError() = Cesium::CesiumTool::computeGeometryError(rectangle);
	tileset->root()->boundingVolume()->region() = Cesium::CesiumTool::rect2box(rectangle);
	tileset->root()->content()->uri() = name + ".pnts";
	OC_WRITE_Tiltset(tileset, path + name + ".json");


	{
		String b3dmFile = path + name + ".b3dm";

		//writer.param.writeGltf = true;
		CesiumTool::writeB3DM(*group, b3dmFile);

		tileset->root()->content()->uri() = name + ".b3dm";
		OC_WRITE_Tiltset(tileset, path + name + "_line.json");

		//osg::ref_ptr<TDTILES::Tile> tile = new TDTILES::Tile();
		//tile->content()->uri() = name + ".b3dm";
		//tile->geometricError() = tileset->geometricError();
		//tile->boundingVolume()->region() = tileset->root()->boundingVolume()->region();
		//tile->refine() = tileset->root()->refine() = TDTILES::REFINE_ADD;
		//tileset->root()->children().push_back(tile);
	}

}

void toi3dm(OC::Json::Value & root)
{
	Vector3List locations;
	locations.push_back(CVector3(120.0, 30.0, 0));
	locations.push_back(CVector3(120.1, 30.1, 0));
	locations.push_back(CVector3(120.0, 30.1, 0));
	locations.push_back(CVector3(120.1, 30.0, 0));


	CRectangle rectangle;
	for (auto location : locations)
	{
		rectangle.merge(CVector2(location.x, location.y));
	}

	CVector2 center = rectangle.getCenter();

	osg::ref_ptr<Cesium::I3dmWriter> i3dmWriter = new Cesium::I3dmWriter;
	osg::Matrix matrix = ProjectTool::computeTransform(CVector3(center.x, center.y,-0.05));
	osg::Vec3 wolrdPosition = matrix.getTrans();

	auto calc_rn = [&wolrdPosition ,&i3dmWriter](CVector3 location){
		CVector3 position = ProjectTool::toWorld(location);
		CVector3 normalUp = position.normalisedCopy();
		CVector3 normalRight = CVector3::UNIT_Z.crossProduct(position).normalisedCopy();
		i3dmWriter->push(Vector3ToVec3d(position) - wolrdPosition, Vector3ToVec3d(normalUp), Vector3ToVec3d(normalRight));
		return true;
	};

	for (auto location : locations)
	{
		calc_rn(location);
	}

	calc_rn(CVector3(center.x, center.y,0.0));

	String filename = JsonPackage::getString(root, "filename");
	String name, ext, path;
	StringUtil::splitFullFilename(filename, name, ext, path);
	String i3dmFile = path + name + ".i3dm";

	if (ext == "gltf" || ext == "glb")
	{
		i3dmWriter->setUrl(osgDB::getSimpleFileName(filename));
		i3dmWriter->writeToFile(i3dmFile);

		osg::ref_ptr<TDTILES::Tileset> tileset = OC_NEW_Tiltset();
		tileset->root()->transform() = osg::Matrix::translate(matrix.getTrans());
		tileset->geometricError() = tileset->root()->geometricError() = Cesium::CesiumTool::computeGeometryError(rectangle);
		tileset->root()->boundingVolume()->region() = Cesium::CesiumTool::rect2box(rectangle);
		tileset->root()->content()->uri() = name + ".i3dm";
		OC_WRITE_Tiltset(tileset, path + name + ".json");
	}
	else
	{
		bool writeGltf = JsonPackage::getBool(root, "writeGltf",true);
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename);
		if (node.valid())
		{
			if (writeGltf)
			{
				String uri = name + ".gltf";
				String gltfFile = path + uri;
				if (!FileUtil::FileExist(gltfFile))
				{
					CesiumTool::writeGLTF(*node, gltfFile);
				}

				i3dmWriter->setUrl(uri);
			}
			else
			{
				i3dmWriter->setNode(node);
			}

			i3dmWriter->writeToFile(i3dmFile);

			osg::ref_ptr<TDTILES::Tileset> tileset = OC_NEW_Tiltset();
			tileset->root()->transform() = osg::Matrix::translate(matrix.getTrans());
			tileset->root()->boundingVolume()->region() = Cesium::CesiumTool::rect2box(rectangle);
			tileset->geometricError() = tileset->root()->geometricError() = Cesium::CesiumTool::computeGeometryError(rectangle);
			tileset->root()->content()->uri() = name + ".i3dm";
			OC_WRITE_Tiltset(tileset, path + name + ".json");
		}
	}
}

void tokml(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");

	//String ext = osgDB::getLowerCaseFileExtension(filename);
	//String outFilename = StringUtil::replaceAll(filename, "." + ext, ".kml");
	//if (ext == "thunder")
	//{
	//}
	//else if (ext == "radarstation")
	//{
	//	outFilename = StringUtil::replaceAll(filename, "." + ext, "_local.kml");
	//	stationToKML(filename, outFilename, false);
	//	outFilename = StringUtil::replaceAll(filename, "." + ext, "_nation.kml");
	//	stationToKML(filename, outFilename, true);
	//}
	stationToKML2(filename);
}

void hangji()
{
	struct planeData
	{
		double lat;
		double lon;
		double U;
		double V;
		double angle;
		double height;
		double hour;
		double min;
		double sec;
		String departure;
		String destination;
	};
	typedef std::vector<planeData> PlaneDataList;
	std::map<String, PlaneDataList> flyMap;

	String filename = "I:/Project/航迹.csv";
	osgDB::ifstream ifs(filename.c_str());
	if (ifs)
	{
		String line;
		getline(ifs, line);
		getline(ifs, line);
		while (getline(ifs, line))
		{
			StringVector sv = StringUtil::splitEx(line, ",");
			if (sv.size() == 13)
			{
				String name = sv[0];
				StringUtil::trim(name);
				planeData data;
				data.lat = StringConverter::parseReal(sv[2]);
				data.lon = StringConverter::parseReal(sv[3]);
				data.U = StringConverter::parseReal(sv[4]);
				data.V = StringConverter::parseReal(sv[5]);
				data.angle = StringConverter::parseReal(sv[6]);
				data.height = StringConverter::parseReal(sv[7]);
				data.departure = sv[8];
				data.destination = sv[9];
				data.hour = StringConverter::parseReal(sv[10]);
				data.min = StringConverter::parseReal(sv[11]);
				data.sec = StringConverter::parseReal(sv[12]);
				flyMap[name].push_back(data);
			}
			else if (sv.size() > 0)
			{
				std::cout << line << std::endl;
			}
			if (line.empty())
			{
				break;
			}
		}

		OC::Json::Value root;
		for (auto fly : flyMap)
		{
			String name = fly.first;
			PlaneDataList list = fly.second;
			for (auto data : list)
			{
				double sec = data.hour * 3600 + data.min * 60 + data.sec;
				OC::Json::Value value;
				value["lon"] = data.lon;
				value["lat"] = data.lat;
				value["height"] = data.height;
				value["secconds"] = sec;
				value["U"] = data.U;
				value["V"] = data.V;
				value["angle"] = data.angle;
				value["departure"] = data.departure;
				value["destination"] = data.destination;
				root[name].append(value);
			}
		}

		String jsonfile = StringUtil::replaceAll(filename, ".csv", ".json");
		osgDB::ofstream ofs(jsonfile.c_str());
		if (ofs)
		{
			OC::Json::FastWriter writer;
			ofs << writer.write(root);
			ofs.close();
		}

	}

	ifs.close();
}

void pup()
{
	struct header
	{
		uint16 productCode;
		uint16 productDate;
		uint32 productTime;
		uint32 fileLength;
		uint16 radarCode;
		uint16 receiveCode;
		uint16 blockCount;
	};
	
	struct headerProduct
	{
		uint16 h1;
		uint16 h2;
		uint16 h3;
		uint16 h4;
		uint16 h5;
		uint16 h6;
		uint16 h7;
	};

	String filename = "I:/Project/ecoweather/Z_RADR_I_Z9717_20200712181200_P_DOR_SA_CR_10X10_230_NUL.717.bin";
	osgDB::ifstream ifs(filename.c_str(), std::ios::binary);

	if (ifs)
	{
		header header1;
		ifs.read((char*)&header1, 18);
		ifs.close();
	}
}


//void test()
//{
//	//TestKriging();
//}