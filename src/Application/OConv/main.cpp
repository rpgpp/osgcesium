#include "ConvCommand.h"

int main(int argc, char **argv)
{
	if (argc < 1)
	{
		return -1;
	}

	initEnviroment();

	String jsonStr = argv[1];
	for (int i = 2; i < argc; i++) 
	{
		jsonStr += " " + String(argv[i]);
	}
	try
	{
		OC::Json::Reader reader;
		OC::Json::Value root;
		if (reader.parse(jsonStr, root))
		{
			DWORD t1 = GetTickCount();
			String function = JsonPackage::getString(root, "function");
			if (function == "obj23dtiles")
			{
				obj23dtiles(root);
			}
			if (function == "oblique23dtiles")
			{
				oblique23dtiles(root);
			}
			else if (function == "pipe23dtiles")
			{
				pipe23dtiles(root);
			}
			else if (function == "obj2gltf")
			{
				obj2gltf(root);
			}
			else if (function == "conv2obj")
			{
				conv2obj(root);
			}
			else if (function == "offset2origin")
			{
				offset2origin(root);
			}
			else if (function == "csv23dtiles")
			{
				csv23dtiles(root);
			}
			else if (function == "nc2raw")
			{
				nc2raw(root);
			}
			else if (function == "computeTexture")
			{
				computeTexture(root);
			}
			else if (function == "OCZip")
			{
				OCZip(root);
			}
			else if (function == "PackGroup")
			{
				PackGroup(root);
			}
			else if (function == "PackJson")
			{
				PackJson(root);
			}
			else if (function == "npy2txt")
			{
				npy2txt(root);
			}
			else if (function == "Packmetadata")
			{
				Packmetadata(root);
			}
			else if (function == "toi3dm")
			{
				toi3dm(root);
			}
			else if (function == "decodekey")
			{
				decodekey(jsonStr);
			}
			else if (function == "encodekey")
			{
				encodekey(jsonStr);
			}
			else if (function == "biosuuid")
			{
				biosuuid(jsonStr);
			}
			else if (function == "test")
			{
				test();
			}
			else if (function == "tokml")
			{
				tokml(root);
			}
			else if (function == "regiter")
			{
				regiter(jsonStr);
			}
			else if (function == "shp23dtiles")
			{
				shp23dtiles(root);
			}
			else if (function == "json2ShpTable")
			{
				json2ShpTable(root);
			}
			else if (function == "toVolume")
			{
				toVolume(root);
			}
			else if (function == "toVolume")
			{
				toVolume(root);
			}
			else if (function == "json2Mxd")
			{
				json2Mxd(root);
			}
			else if (function == "mapcacher")
			{
				mapcaching(root);
			}

			DWORD t2 = GetTickCount();
			std::cout << function << " time elapse:" << (t2 - t1) * 1e-3 << "s" << std::endl;
		}
	}
	catch (...)
	{

	}
}
