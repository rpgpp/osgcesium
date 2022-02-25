#include "GeographyTool.h"
#include "ConvCommand.h"

void GeographyTool::json2Mxd(String filename)
{

}

inline String getJsonU8(OC::Json::Value& v, String name)
{
	return osgDB::convertStringFromUTF8toCurrentCodePage(JsonPackage::getString(v, name));
}

class TemplateTable1
{
public:

	void readInfo(OC::Json::Value& json)
	{
		if (!json.isNull())
		{
			nameValues["钻孔编号"] = getJsonU8(json, "zkId");
			nameValues["工程编号"] = getJsonU8(json, "proId");
			nameValues["钻孔类型"] = getJsonU8(json, "zkType");
			nameValues["孔深"] = getJsonU8(json, "d");
			nameValues["钻孔坐标X"] = getJsonU8(json, "x");
			nameValues["钻孔坐标Y"] = getJsonU8(json, "y");
		}
	}

	void readInfo0(OC::Json::Value& json)
	{
		if (!json.isNull())
		{
			String location = getJsonU8(json, "location");
			int size = (int)location.length() / 12;
			for (int i = 0; i < size; i++)
			{
				location.insert((i + 1) * 12, "$");
			}
			nameValues["钻孔位置"] = location;
		}
	}

	void readInfo2(OC::Json::Value& json)
	{
		if (!json.isNull())
		{
			nameValues["地面标高"] = getJsonU8(json, "h");
			nameValues["初见水位埋深"] = getJsonU8(json, "cjsw");
			nameValues["开孔日期"] = getJsonU8(json, "startTime");
			nameValues["终孔日期"] = getJsonU8(json, "endTime");
			nameValues["制图"] = getJsonU8(json, "jvry");
			nameValues["审核"] = getJsonU8(json, "jcry");
			nameValues["工程日期"] = getJsonU8(json, "proDate");
		}
	}

	void readBlockList1(OC::Json::Value& json)
	{
		if (!json.isNull())
		{
			std::map<float, ShpRow> mapRow;
			for (auto& block : json)
			{
				String c1 = getJsonU8(block, "dh");
				String h1 = getJsonU8(block, "dbH1");
				String h2 = getJsonU8(block, "dbH2");
				float top = StringConverter::parseReal(h1);
				float bottom = StringConverter::parseReal(h2);
				float height = bottom - top;
				String c3 = StringConverter::formatDoubletoString(height, 1);
				String c4 = getJsonU8(block, "tcName");
				String c5 = getJsonU8(block, "discribe");
				int size = (int)c5.length() / 48;
				for (int i = 1; i < size; i++)
				{
					c5.insert((i + 1) * 48, "$");
				}

				ShpRow row;
				row.mHeight = height;
				{
					ShpCol col(1.0, c1);
					row.pushCol(col);
				}
				{
					ShpCol col(1.0, h2);
					row.pushCol(col);
				}
				{
					ShpCol col(1.0, c3);
					row.pushCol(col);
				}
				{
					ShpCol col(1.0, c4);
					col.mFill = true;
					row.pushCol(col);
				}

				{
					ShpCol col(0.0, c5);
					row.pushCol(col);
				}

				mapRow[bottom] = row;
			}

			for (auto& it : mapRow)
			{
				ShpRow& r = it.second;
				if (!rowList.empty())
				{
					ShpRow& upRow = rowList[rowList.size() - 1];
					if (upRow.colSize() == r.colSize())
					{
						for (int i = 0; i < upRow.colSize(); i++)
						{
							ShpCol& col1 = upRow.mColumnList[i];
							ShpCol& col2 = r.mColumnList[i];
							if (col1.mText == col2.mText &&
								col2.mWidth == col1.mWidth)
							{
								col1.mMerged = true;
								col2.mMerged = true;
							}
						}
					}
				}
				rowList.push_back(r);
			}
		}
	}

	void readBlockList2(OC::Json::Value& json)
	{

	}

	void readBlockList(OC::Json::Value& json)
	{
		String zkType = getType();
		StringStream sstream;
		if (zkType == "地质勘察孔")
		{
			String templateFile = Singleton(Environment).getAppDir() + "config\\柱状图模板\\地质勘探孔.json";
			std::ifstream ifs(templateFile.c_str());
			if (ifs)
			{
				String line;
				while (getline(ifs, line))
				{
					sstream << osgDB::convertStringFromUTF8toCurrentCodePage(line);
				}
				ifs.close();
			}

			readBlockList1(json);
		}
		else
		{
			String templateFile = Singleton(Environment).getAppDir() + "config\\柱状图模板\\水文勘探孔.json";
			std::ifstream ifs(templateFile.c_str());
			if (ifs)
			{
				String line;
				while (getline(ifs, line))
				{
					sstream << osgDB::convertStringFromUTF8toCurrentCodePage(line);
				}
				ifs.close();
			}
			readBlockList2(json);
		}

		String templateText = fillTemplate(sstream.str());
		mTemplateText = A2U8(templateText);
	}

	String getType()
	{
		return nameValues["钻孔类型"];
	}

	String fillTemplate(String templatStr)
	{
		String lastRow = "工程编号：{工程编号}          制图：{制图}          审核：{审核}               日期：{工程日期}";
		String templateText = templatStr;
		for (auto& it : nameValues)
		{
			String name = it.first;
			String value = it.second;
			templateText = StringUtil::replaceAll(templateText, "{" + name + "}", value);
			lastRow = StringUtil::replaceAll(lastRow, "{" + name + "}", value);
		}

		if (mHasLastRow)
		{
			OC::ShpRow row;
			OC::ShpCol col(0.0, lastRow);
			row.mHeight = 0.5;
			row.pushCol(col);
			rowList.push_back(row);
		}

		return templateText;
	}
	bool				mHasLastRow = false;
	String				mTemplateText;
	ShpRowList			rowList;
	NameValuePairList	nameValues;
};


void json2Mxd(OC::Json::Value& root)
{
	bool zip = JsonPackage::getBool(root, "zip", true);
	bool delWhenFinish = JsonPackage::getBool(root, "delWhenFinish", true);
	String filename = JsonPackage::getString(root, "filename");
	String outputPath = JsonPackage::getString(root, "output");
	String templateMxd = JsonPackage::getString(root, "mxd");
	String name, ext, path;
	StringUtil::splitFullFilename(filename, name, ext, path);
	if (outputPath.empty())
	{
		outputPath = StringUtil::replaceAll(filename, ".json", "");
	}
	if (templateMxd.empty())
	{
		templateMxd = Singleton(Environment).getAppDir() + "config/柱状图模板/Template.mxd";
	}

	TemplateTable1 table;
	std::ifstream ifs(filename.c_str());
	if (ifs)
	{
		OC::Json::Value root;
		OC::Json::Reader reader;
		if (reader.parse(ifs, root))
		{
			table.readInfo0(root);
			table.readInfo(root["proZkBaseEntity"]);
			table.readInfo2(root["proZkDiscribeEntity"]);
			table.readBlockList(root["proZkBlEntityList"]);

		}
		ifs.close();
	}

	{
		OC::Json::Value root;
		OC::Json::Reader reader;
		if (reader.parse(table.mTemplateText, root))
		{
			ShpDrawerHelper drawerHelper;
			drawerHelper.readFile(root);
			drawerHelper.addRows(table.rowList);
			drawerHelper.output(outputPath);
			String outmxd = outputPath + "/" + name + ".mxd";
			if (!templateMxd.empty() && !delWhenFinish)
			{
				osgDB::copyFile(templateMxd, outmxd);
			}
			if (zip)
			{
				String zipfilename = path + "/" + name + ".zip";
				if (zipFile f = zipOpen(zipfilename.c_str(), APPEND_STATUS_CREATE))
				{
					auto compressFile = [&f](String filename, String file)
					{
						if (FileUtil::FileExist(filename))
						{
							FileStreamDataStream fStream(new std::ifstream(filename.c_str(), std::ios::binary));
							int size = (int)fStream.size();
							MemoryDataStreamPtr srcStream(new MemoryDataStream(size));
							fStream.read(srcStream->getPtr(), size);
							uLong destSize;
							if (ZIP_OK == zipOpenNewFileInZip(f, file.c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
							{
								if (ZIP_OK == zipWriteInFileInZip(f, srcStream->getPtr(), size))
								{
									zipCloseFileInZip(f);
								}
							}
						}
					};
					StringVector sv;
					sv.push_back("point");
					sv.push_back("line");
					sv.push_back("polygon");
					StringVector sv2;
					sv2.push_back("dbf");
					sv2.push_back("prj");
					sv2.push_back("shp");
					sv2.push_back("shx");
					for (auto name : sv)
					{
						for (auto ext : sv2)
						{
							String file = name + "." + ext;
							String filename = outputPath + "/" + file;
							compressFile(filename, file);
							if (delWhenFinish)
							{
								FileUtil::FileDelete(filename);
							}
						}
					}
					compressFile(templateMxd, name + ".mxd");
					if (delWhenFinish)
					{
						bool ret = FileUtil::DirDelete(outputPath);
						std::cout << ret;
					}
					zipClose(f, 0);
				}
			}
		}
	}
}

void json2ShpTable(OC::Json::Value& root)
{
	String filename = JsonPackage::getString(root, "filename");
	String outputPath = JsonPackage::getString(root, "output");
	String templateMxd = JsonPackage::getString(root, "mxd");
	String name, ext, path;
	StringUtil::splitFullFilename(filename, name, ext, path);
	ShpDrawerHelper drawerHelper;
	drawerHelper.readFile(filename);
	if (outputPath.empty())
	{
		outputPath = StringUtil::replaceAll(filename, ".json", "");
	}
	drawerHelper.output(outputPath);
	String outmxd = outputPath + "/" + name + ".mxd";
	osgDB::copyFile(templateMxd, outmxd);
}


struct C2
{
	int id;
	int rygid;
	float head;
	float distance;
	int corner;
	CVector2 coord;
};

struct C1
{
	int id;
	CVector2 coners[5];
	std::vector<C2> dataList;
	String spatial;
};

void convMm()
{
	String a = "+proj=tmerc +lat_0=0 +lon_0=117 +k=1 +x_0=500000 +y_0=0 +ellps=IAU76 +units=m +no_defs";
	String b = "+proj=tmerc +lat_0=0 +lon_0=123 +k=1 +x_0=500000 +y_0=0 +ellps=IAU76 +units=m +no_defs";

	std::map<int, C1> placeMap;
	{
		String filename = "I:/Project/华勘/样地角点测量记录表33.csv";
		std::ifstream ifs(filename.c_str());
		if (ifs)
		{
			String line;
			getline(ifs, line);
			while (getline(ifs, line))
			{
				line = A2U8(line);
				StringVector sv = StringUtil::split(line, ",");
				int id = StringConverter::parseInt(sv[2]);
				String loc = sv[4];
				String yStr = sv[5];
				if (StringUtil::startsWith(yStr, "21", false))
				{
					placeMap[id].spatial = b;
				}
				else
				{
					placeMap[id].spatial = a;
				}
				yStr = yStr.substr(2, yStr.length() - 2);
				if (StringUtil::startsWith(loc, "4"))
				{

					placeMap[id].coners[4].x = StringConverter::parseReal(sv[6]);
					placeMap[id].coners[4].y = StringConverter::parseReal(yStr);
				}
				else if (StringUtil::startsWith(loc, "1"))
				{
					placeMap[id].coners[1].x = StringConverter::parseReal(sv[6]);
					placeMap[id].coners[1].y = StringConverter::parseReal(yStr);
				}
				else if (StringUtil::startsWith(loc, "2"))
				{
					placeMap[id].coners[2].x = StringConverter::parseReal(sv[6]);
					placeMap[id].coners[2].y = StringConverter::parseReal(yStr);
				}
				else if (StringUtil::startsWith(loc, "3"))
				{
					placeMap[id].coners[3].x = StringConverter::parseReal(sv[6]);
					placeMap[id].coners[3].y = StringConverter::parseReal(yStr);
				}
			}
			ifs.close();
		}
	}

	String filename = "I:/Project/华勘/每木检尺记录表.csv";
	std::ifstream ifs(filename.c_str());
	if (ifs)
	{
		String line;
		getline(ifs, line);
		while (getline(ifs, line))
		{
			StringVector sv = StringUtil::splitEx(line, ",");
			C2 c;
			c.rygid = StringConverter::parseInt(sv[0]);
			c.id = StringConverter::parseInt(sv[2]);
			c.head = StringConverter::parseReal(sv[16]);
			c.distance = StringConverter::parseReal(sv[17]);
			c.corner = StringConverter::parseInt(sv[19]);
			placeMap[c.id].dataList.push_back(c);
		}
		ifs.close();
	}
	Vector2List list;
	String path = osgDB::getFilePath(filename);
	for (auto& m : placeMap)
	{
		String filename = path + "/" + StringConverter::toString(m.first) + ".txt";
		std::ofstream ofs(filename);
		C1& c1 = m.second;
		MapProject project(c1.spatial);

		ofs << A2U8(String("样地角点")) << std::endl;
		for (int i = 1; i < 5; i++)
		{
			CVector2 v = c1.coners[i];
			ofs << i << " "
				<< StringConverter::formatDoubletoString(v.x, 2) << " "
				<< StringConverter::formatDoubletoString(v.y, 2) << " ";
			CVector2 v2 = project.localTo4326(CVector2(v.y, v.x));
			ofs << StringConverter::formatDoubletoString(v2.x, 6) << " "
				<< StringConverter::formatDoubletoString(v2.y, 6) << std::endl;
			list.push_back(v2);
		}
		ofs << A2U8(String("每木")) << std::endl;
		for (auto& d : c1.dataList)
		{
			float head = Math::DegreesToRadians(d.head);
			int corner = d.corner;
			CVector2 origin = c1.coners[corner];
			CVector2 v;
			int dx = (corner == 1 || corner == 2) ? -1 : 1;
			int dy = (corner == 2 || corner == 3) ? -1 : 1;
			v.x = fabs(cos(head)) * dx * d.distance + origin.x;
			v.y = fabs(sin(head)) * dy * d.distance + origin.y;
			ofs << d.rygid << " ";
			ofs << StringConverter::formatDoubletoString(v.x, 2) << " "
				<< StringConverter::formatDoubletoString(v.y, 2) << " ";

			CVector2 v2 = project.localTo4326(CVector2(v.y, v.x));
			ofs << StringConverter::formatDoubletoString(v2.x, 6) << " "
				<< StringConverter::formatDoubletoString(v2.y, 6) << std::endl;

			list.push_back(v2);
		}

		ofs.close();
	}

	//CShpDrawer drawer;
	//drawer.test(list);
}
