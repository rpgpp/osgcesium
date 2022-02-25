#include "ConvCommand.h"
#include "OCesium/BuildingConverter.h"

using namespace OC;
using namespace OC::Cesium;
using namespace OC::Modeling;

inline void readGLTFConfig(OC::Json::Value& root)
{
	Cesium::gltfConfig::sectionSize = JsonPackage::getInt(root, "sectionSize", 0);
	Cesium::gltfConfig::quantizationbits = JsonPackage::getInt(root, "quantizationbits", 16);
	Cesium::gltfConfig::AtlasImageSize = JsonPackage::getInt(root, "AtlasImageSize", 0);
	Cesium::gltfConfig::MaxTextureSize = JsonPackage::getInt(root, "MaxTextureSize", 0);
	Cesium::gltfConfig::writeGltf = JsonPackage::getBool(root, "writeGltf", false);
	Cesium::gltfConfig::prettyPrint = JsonPackage::getBool(root, "prettyPrint", false);
	Cesium::gltfConfig::disableLight = JsonPackage::getBool(root, "disableLight", false);
	Cesium::gltfConfig::mergeClampTexture = JsonPackage::getBool(root, "mergeClampTexture", true);
	Cesium::gltfConfig::mergeRepeatTexture = JsonPackage::getBool(root, "mergeRepeatTexture", false);
	Cesium::gltfConfig::enableWebpCompress = JsonPackage::getBool(root, "enableWebpCompress", false);
	Cesium::gltfConfig::enableKtxCompress = JsonPackage::getBool(root, "enableKtxCompress", false);
	Cesium::gltfConfig::enableDracoCompress = JsonPackage::getBool(root, "enableDracoCompress", false);
	Cesium::gltfConfig::forceDoubleSide = JsonPackage::getBool(root, "forceDoubleSide", false);
	Cesium::gltfConfig::geometryErroRatio = JsonPackage::getDouble(root, "geometryErroRatio", 1.0);
	
	std::cout << "sectionSize:" << Cesium::gltfConfig::sectionSize << std::endl;
	std::cout << "quantizationbits:" << Cesium::gltfConfig::quantizationbits << std::endl;
	std::cout << "mergeClampTexture:" << Cesium::gltfConfig::mergeClampTexture << std::endl;
	std::cout << "mergeRepeatTexture:" << Cesium::gltfConfig::mergeRepeatTexture << std::endl;
	std::cout << "enableWebpCompress:" << Cesium::gltfConfig::enableWebpCompress << std::endl;
	std::cout << "enableKtxCompress:" << Cesium::gltfConfig::enableKtxCompress << std::endl;
	std::cout << "enableDracoCompress:" << Cesium::gltfConfig::enableDracoCompress << std::endl;
	std::cout << "forceDoubleSide:" << Cesium::gltfConfig::forceDoubleSide << std::endl;
	std::cout << "disableLight:" << Cesium::gltfConfig::disableLight << std::endl;
	std::cout << "AtlasImageSize:" << Cesium::gltfConfig::AtlasImageSize << std::endl;
	std::cout << "MaxTextureSize:" << Cesium::gltfConfig::MaxTextureSize << std::endl;
}

void obj23dtiles(OC::Json::Value& root)
{
	readGLTFConfig(root);

	String filename = JsonPackage::getString(root, "filename");
	String basename, extension, out_dir;
	StringUtil::splitFullFilename(filename, basename, extension, out_dir);
	glTFConverter mGLTFConvertor;
	mGLTFConvertor.localOffset = JsonPackage::getBool(root, "localOffset");
	mGLTFConvertor.upAxisZ = JsonPackage::getBool(root, "upAxisZ", true);
	mGLTFConvertor.rotate = JsonPackage::getBool(root, "rotate", false);
	mGLTFConvertor.readPrjFile(JsonPackage::getString(root, "prjfile"));
	mGLTFConvertor.mPrjOffset = StringConverter::parseVector3(JsonPackage::getString(root, "prjoffset"));
	mGLTFConvertor.outputPath = JsonPackage::getString(root, "output");
	String offsetStr = JsonPackage::getString(root, "offset");
	mGLTFConvertor.setLocaltion(StringConverter::parseVector3(offsetStr));
	mGLTFConvertor.readNodeFile(filename);
	String csvfile = JsonPackage::getString(root, "table");
	if (FileUtil::FileExist(csvfile))
	{
		mGLTFConvertor.readAttributeMap(csvfile);
	}
	mGLTFConvertor.output();
}

void oblique23dtiles(OC::Json::Value& root)
{
	readGLTFConfig(root);
	int threadNum = JsonPackage::getInt(root, "threadNum", 10);

	String filename = JsonPackage::getString(root, "filename");
	String basename, extension, out_dir;
	StringUtil::splitFullFilename(filename, basename, extension, out_dir);
	ObliqueConverter converter(threadNum);
	converter.mRWriteJson = JsonPackage::getBool(root, "rwrite", true);
	converter.mRWriteB3dm = JsonPackage::getBool(root, "rwrite", true);
	String offsetStr = JsonPackage::getString(root, "location", "108.9594 34.2196 0.0");
	converter.mLocation = StringConverter::parseVector3(offsetStr);
	converter.mDestDir = JsonPackage::getString(root, "output");
	converter.mergeTop = JsonPackage::getBool(root, "mergeTop");
	converter.mDebugDepth = JsonPackage::getInt(root, "debugDepth", 100);
	converter.mMaxTileNodeNum = JsonPackage::getInt(root, "maxTileNodeNum", 32);
	converter.mMaxRootTileNodeNum = JsonPackage::getInt(root, "maxRootTileNodeNum", 64);
	
	converter.readNode(filename);
}

void obj2gltf(OC::Json::Value& root)
{
	readGLTFConfig(root);
	String filename = JsonPackage::getString(root, "filename");
	glTFConverter mGLTFConvertor;
	mGLTFConvertor.rotate = JsonPackage::getBool(root, "rotate");
	mGLTFConvertor.localOffset = JsonPackage::getBool(root, "localOffset");
	mGLTFConvertor.outputPath = JsonPackage::getString(root, "output");

	if (mGLTFConvertor.readNodeFile(filename))
	{
		if (JsonPackage::getBool(root, "glb"))
		{
			mGLTFConvertor.writeToGlb();
		}
		else
		{
			mGLTFConvertor.writeToGltf();
		}
	}
}

void pipe23dtiles(OC::Json::Value& root)
{
	readGLTFConfig(root);
	String filename = JsonPackage::getString(root, "filename");

	if (StringUtil::endsWith(filename, "/", false))
	{
		filename = filename.substr(0, filename.length() - 1);
	}

	String output = JsonPackage::getString(root, "output");

	String name, ext, path;
	StringUtil::splitFullFilename(filename, name, ext, path);
	String HostVal = path;
	String configFile = HostVal + "/pipe.xml";

	String nodePrjFile = filename + "_node.prj";

	if (!FileUtil::FileExist(nodePrjFile))
	{
		return;
	}

	ModelDataManager mgr(true);
	mgr.loadConfig(Singleton(Environment).getAppDir() + "config/pipe_config.xml");

	StringStream sstream;
	String driver = "shapefile";
	String UserVal = "";
	String PswVal = "";
	String DatabaseVal = "";
	sstream << "driver:" << driver << ";ip:" << HostVal << ";catalog:" << DatabaseVal << ";user:" <<
		UserVal << ";password:" << PswVal;
	String connect = sstream.str();
	if (mgr.connect(sstream.str()) && mgr.loadConfig(configFile))
	{
		String fext = "b3dm";
		SaveStrategyType sst = SAVE_TILESET;
		mgr.setExtension(fext);
		mgr.setSaveStrategy(sst);

		if (output.empty())
		{
			output = Singleton(Environment).getTempDir();
		}

		CORE_MAKE_DIR(output);
		mgr.setSavePath(output);
		StringVector sv;
		sv.push_back(name);
		mgr.setList(sv);
		mgr.setMergeTile(true);
		mgr.setOptimizeLevel(4);
		mgr.setConstElevation(0.0);
		String IputPrjCS;
		ifstream ifs(nodePrjFile.c_str());
		getline(ifs, IputPrjCS);
		mgr.getKeyConfig().setConfig("prjcs", IputPrjCS);
		ifs.close();
		mgr.build();
	}
}

void shp23dtiles(OC::Json::Value& root)
{
	readGLTFConfig(root);
	String filename = JsonPackage::getString(root, "filename");

	BuildingConverter conveter;
	conveter.outputPath = JsonPackage::getString(root, "output");
	conveter.mErrors = JsonPackage::getString(root, "errors");
	conveter.splitLevel = JsonPackage::getInt(root, "depth");
	conveter.constantHeight = JsonPackage::getDouble(root, "constantHeight");
	conveter.heightExpression = JsonPackage::getString(root, "heightExpression");
	conveter.nameExpression = JsonPackage::getString(root, "nameExpression");
	conveter.resource_lib = JsonPackage::getString(root, "resource_lib");
	conveter.mMaxNodeInTile = JsonPackage::getInt(root, "maxNodeInTile",1500);
	
	for (int i = 1; i < 100; i++)
	{
		String attr = Stringify() << "attr" << i;
		String name = JsonPackage::getString(root, attr);
		if (!name.empty())
		{
			conveter.mWriteAttributeList.push_back(name);
		}
	}

	conveter.readShp(filename);
	conveter.output();
}