#include "ConvCommand.h"
#include "Geography/Kriging.h"
#include "Geography/GDALTool.h"
#include "SqliteOper.h"
#include "OCHttpClient/HttpClient.h"

void testLayerJson()
{
    String filename = Singleton(Environment).getAppDir() + "config/CesiumTerrain.json";
    std::ifstream ifs(filename);
    if (ifs)
    {
        OC::Json::Value root;
        OC::Json::Reader reader;
        if (reader.parse(ifs, root))
        {
            int level = 0;
            OC::Json::Value available = root["available"];
            OC::Json::Value::iterator it = available.begin();
            for (; it != available.end(); it++) {
                OC::Json::Value tile = *it;
                String line = osgEarth::Stringify() << "Level:" << level++ << " Count:" << tile.size();
                std::cout << line << std::endl;
            }
        }
        ifs.close();
    }
}

void test()
{
    //GDALTool tool;
    //tool.testContour("E:/ArcGISFolder/mt_everest_90m1.png","I:/iso.shp",100.0);
    //tool.testIDW("I:/testidw.txt");

    // createSqlite();
    // testLayerJson();
    String once = StringUtil::GenGUID();
    String timestamp = osgEarth::Stringify() << time(0) * 1000;// -60000 * 15;
    StringMap sm;
    sm["dataCode"] = "SURF_CHN_MUL_HOR";
    sm["dataFormat"] = "json";
    sm["elements"] = "Station_Name,Station_Id_C,Datetime,PRS,TEM,TEM_Max,TEM_Min,PRE_24h,WIN_S_Avg_2mi,WIN_D_INST_Max,WIN_S_Inst_Max,GST,GST_Max,GST_Min,VIS_HOR_10MI,WEP_Now";
    sm["interfaceid"] = "getSurfEleByTimeAndStaID";
    sm["nonce"] = once;
    sm["serviceNodeId"] = "NMIC_MUSIC_CMADAAS";
    sm["staIds"] = "57719";
    sm["times"] = "20220221060000";
    sm["timestamp"] = timestamp;
    sm["userId"] = "USR_BCWH_TEST";
    sm["pwd"] = "TESTcmadaas@0629";

    String hostUrl = "http://10.104.90.120:80/music-ws/api?";

    int count = 0;
    StringStream sstream;
    for (const auto& it : sm)
    {
        String v = it.second;
        if (!v.empty())
        {
            if (count > 0)
            {
                sstream << "&";
            }
            sstream << it.first << "=" << it.second;
            count++;
        }
    }

    String paramsStri = sstream.str();
    String paramsHash = StringUtil::hashMD5(paramsStri);
    StringUtil::toUpperCase(paramsHash);

    StringStream sstream2;
    {
        count = 0;
        sm["pwd"] = "";
        for (const auto& it : sm)
        {
            String v = it.second;
            if (!v.empty())
            {
                if (count > 0)
                {
                    sstream2 << "&";
                }
                sstream2 << it.first << "=" << it.second;
                count++;
            }
        }
    }

    String url = osgEarth::Stringify() << hostUrl << sstream2.str() << "&sign=" << paramsHash;
    OC::HTTPRequest request(url);
    osgEarth::ReadResult result = OC::HTTPClient::readString(request);
    if (result.code() == 0)
    {
        String s = result.getString();
        s = osgDB::convertStringFromUTF8toCurrentCodePage(s);
        std::cout << s << std::endl;
    }
}

