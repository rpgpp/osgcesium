#include "MapCacher.h"
#include "OCMain/ConfigManager.h"
#include "OCMain/LogManager.h"
#include "curl/curl.h"
#include "OCMain/DataStream.h"
#include "OCMain/JsonPackage.h"

namespace OC
{
    MapCacher* msSingleton = NULL;
    CORE_MUTEX(mutex)

    const String userAgent = "Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.48 Safari/537.36";

    MapCacher* MapCacher::getSingletonPtr()
    {
        if (msSingleton == NULL)
        {
            msSingleton = new MapCacher;
        }

        return msSingleton;
    }

    MapCacher& MapCacher::getSingleton()
    {
        if (msSingleton == NULL)
        {
            CORE_LOCK_MUTEX(mutex)
            if (msSingleton == NULL)
            {
                msSingleton = new MapCacher;
            }
        }

        return *msSingleton;
    }

    MapCacher::MapCacher()
        :mToken("2a0e637a8772d92b123ee8866dee4a82")
    {
        String path1 = Singleton(OC::ConfigManager).getStringValue("MapCacheDir", "d:/map_caches/");
        path1 = osgDB::convertFileNameToUnixStyle(path1);
        if (path1[path1.length() - 1] != '/')
        {
            path1 += "/";
        }
        mCacheRoot.push_back(path1);

        for (int i = 2; i < 5; i++)
        {
            String val = osgEarth::Stringify() << "MapCacheDir" << i;
            String path2 = Singleton(OC::ConfigManager).getStringValue(val);
            if (path2.empty())
            {
                continue;
            }
            path2 = osgDB::convertFileNameToUnixStyle(path2);
            if (path2[path2.length() - 1] != '/')
            {
                path2 += "/";
            }
            mCacheRoot.push_back(path2);
        }

        msSingleton = this;

        // mCesiumHttpClient.setReferer("https://sandcastle.cesium.com/");
        mTDTHttpClient.setReferer("www.tianditu.gov.cn");
        //OC::HTTPClient::getClient().setReferer("www.tianditu.gov.cn");
        OC::HTTPClient::setUserAgent(userAgent);
        OC::HTTPClient::setTimeout(10);
    }

    MapCacher::~MapCacher()
    {

    }

    String template_tdt_url_img = "https://t{s}.tianditu.gov.cn/img_w/wmts?service=wmts&request=GetTile&version=1.0.0&LAYER=img&tileMatrixSet=w&TileMatrix={z}&TileRow={y}&TileCol={x}&style=default&format=tiles";
    String template_tdt_url_cia = "https://t{s}.tianditu.gov.cn/cia_w/wmts?service=wmts&request=GetTile&version=1.0.0&LAYER=cia&tileMatrixSet=w&TileMatrix={z}&TileRow={y}&TileCol={x}&style=default&format=tiles";
    String template_tdt_url_vec = "https://t{s}.tianditu.gov.cn/vec_w/wmts?service=wmts&request=GetTile&version=1.0.0&LAYER=vec&tileMatrixSet=w&TileMatrix={z}&TileRow={y}&TileCol={x}&style=default&format=tiles";
    String template_tdt_url_cva = "https://t{s}.tianditu.gov.cn/cva_w/wmts?service=wmts&request=GetTile&version=1.0.0&LAYER=cva&tileMatrixSet=w&TileMatrix={z}&TileRow={y}&TileCol={x}&style=default&format=tiles";
    String template_tdt_url_ter = "https://t{s}.tianditu.gov.cn/ter_w/wmts?service=wmts&request=GetTile&version=1.0.0&LAYER=ter&tileMatrixSet=w&TileMatrix={z}&TileRow={y}&TileCol={x}&style=default&format=tiles";
    String template_tdt_url_cta = "https://t{s}.tianditu.gov.cn/cta_w/wmts?service=wmts&request=GetTile&version=1.0.0&LAYER=cta&tileMatrixSet=w&TileMatrix={z}&TileRow={y}&TileCol={x}&style=default&format=tiles";
    String template_tdt_url_data = "https://t{s}.tianditu.gov.cn/mapservice/swdx?T=elv_c&x={x}&y={y}&l={z}";
    String template_cesium_url_access = "https://api.cesium.com/v1/assets/1/endpoint?access_token=";
    String template_cesium_url_ter = "https://assets.cesium.com/1/{z}/{x}/{y}.terrain?extensions=octvertexnormals-watermask-metadata&v=1.2.0";
    String template_google_url = "http://104.168.234.50/maps/vt/lyrs={l}@167000000&hl=zh-CN&gl=cn&x={x}&y={y}&z={z}&s=Galil";

    void MapCacher::getTdt()
    {
        
    }

    String MapCacher::getInCachers(String lastfix)
    {
        String filename;
        for (auto path : mCacheRoot)
        {
            filename = path + lastfix;
            if (FileUtil::FileExist(filename))
            {
                return filename;
            }
        }
        return filename;
    }

    bool MapCacher::downloadFile(String url, String filename, StringMap sm, OC::HTTPClient* client)
    {
        OC::HTTPRequest req(url);
        
        for (auto it : sm)
        {
            req.addHeader(it.first, it.second + "\n");
        }
        
        String fakeip = osgEarth::Util::Stringify() 
            << (int)Math::RangeRandom(0, 255) << "."
            << (int)Math::RangeRandom(0, 255) << "."
            << (int)Math::RangeRandom(0, 255) << "."
            << (int)Math::RangeRandom(0, 255);
        req.addHeader("X-Forwarded-For", fakeip + "\n");
        req.addHeader("Connection", "Keep-Alive");
        
        OC::HTTPResponse response = OC::HTTPClient::get(req);
        if (response.isOK())
        {
            if (response.getNumParts() < 1)
                return false;

            unsigned int part_num = response.getNumParts() > 1 ? 1 : 0;
            std::istream& input_stream = response.getPartStream(part_num);

            osgDB::makeDirectoryForFile(filename);
            std::ofstream fout;
            fout.open(filename.c_str(), std::ios::out | std::ios::binary);

            input_stream.seekg(0, std::ios::end);
            int length = input_stream.tellg();
            input_stream.seekg(0, std::ios::beg);
            MemoryDataStream dataStream(length);
            input_stream.read((char*)dataStream.getPtr(), length);
            fout.write((char*)dataStream.getPtr(), length);
            fout.close();
            OCLogStream << "New cache " << filename;
            return true;
        }
        else
        {
            return false;
        }
    }

    String MapCacher::getCesium_terrain(String x, String y, String z)
    {   
        String file = osgEarth::Util::Stringify() << "terrain/" << z << "/" << x << "/" << y;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            {
                CORE_LOCK_MUTEX(mutex)
                time_t t = time(0);
                if (t - mLastTime > 3600)
                {
                    mLastTime = t;
                    mAccessToken = StringUtil::BLANK;
                }
            }

            if (mAccessToken.empty())
            {
                CORE_LOCK_MUTEX(mutex)
                if (mAccessToken.empty())
                {
                    String token = Singleton(ConfigManager).getStringValue("CesiumToken");
                    String url = template_cesium_url_access + token;
                    OC::HTTPRequest req(url);
                    HTTPClient::getClient().setReferer("https://sandcastle.cesium.com/");
                    osgEarth::ReadResult result = HTTPClient::readString(req);
                    if (result.succeeded())
                    {
                        String json = result.getString();
                        OC::Json::Value root;
                        OC::Json::Reader reader;
                        if (reader.parse(json, root))
                        {
                            mAccessToken = JsonPackage::getString(root, "accessToken");
                            mCesiumHeaders["Authorization"] = "Bearer " + mAccessToken;
                            OCLogStream << "Cesium Access Token: " << mAccessToken;
                            mLastTime = time(0);
                        }
                    }
                }
            }

            if (mAccessToken.empty())
            {
                return StringUtil::BLANK;
            }

            String location = template_cesium_url_ter;
            osgEarth::Util::replaceIn(location, "{x}", x);
            osgEarth::Util::replaceIn(location, "{y}", y);
            osgEarth::Util::replaceIn(location, "{z}", z);
            if (!downloadFile(location, filename, mCesiumHeaders, &mCesiumHttpClient))
            {
                time_t currentTime = time(0);
                if (currentTime - mLastTime > 1800)
                {
                    mAccessToken = StringUtil::BLANK;
                    mLastTime = currentTime;
                }
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getTDT_elv_c(String x, String y, String z)
    {
        String file = osgEarth::Util::Stringify() << "elv_c/" << z << "/" << x << "/" << y;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            int s = Math::RangeRandom(0, 8);
            String location = template_tdt_url_data;
            osgEarth::Util::replaceIn(location, "{s}", osgEarth::Util::Stringify() << s);
            osgEarth::Util::replaceIn(location, "{x}", osgEarth::Util::Stringify() << x);
            osgEarth::Util::replaceIn(location, "{y}", osgEarth::Util::Stringify() << y);
            osgEarth::Util::replaceIn(location, "{z}", osgEarth::Util::Stringify() << z);
            location += "&tk=" + mToken;
            if (!downloadFile(location, filename, mTDTHeaders, &mTDTHttpClient))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getTDT_img_w(String x, String y, String z)
    {
        String ext = ".jpg";
        String file = osgEarth::Util::Stringify() << "img_w/" << z << "/" << y << "/" << x << ext;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            int s = Math::RangeRandom(0,8);
            String location = template_tdt_url_img;
            osgEarth::Util::replaceIn(location, "{s}", osgEarth::Util::Stringify() << s);
            osgEarth::Util::replaceIn(location, "{x}", osgEarth::Util::Stringify() << x);
            osgEarth::Util::replaceIn(location, "{y}", osgEarth::Util::Stringify() << y);
            osgEarth::Util::replaceIn(location, "{z}", osgEarth::Util::Stringify() << z);
            location += "&tk=" + mToken + "&f=" + ext;
            if (!downloadFile(location, filename, mTDTHeaders, &mTDTHttpClient))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getTDT_cia_w(String x, String y, String z)
    {
        String ext = ".png";
        String file = osgEarth::Util::Stringify() << "cia_w/" << z << "/" << y << "/" << x << ext;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            int s = Math::RangeRandom(0, 8);
            String location = template_tdt_url_cia;
            osgEarth::Util::replaceIn(location, "{s}", osgEarth::Util::Stringify() << s);
            osgEarth::Util::replaceIn(location, "{x}", osgEarth::Util::Stringify() << x);
            osgEarth::Util::replaceIn(location, "{y}", osgEarth::Util::Stringify() << y);
            osgEarth::Util::replaceIn(location, "{z}", osgEarth::Util::Stringify() << z);
            location += "&tk=" + mToken + "&f=" + ext;
            if (!downloadFile(location, filename, mTDTHeaders, &mTDTHttpClient))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getTDT_vec_w(String x, String y, String z)
    {
        String ext = ".jpg";
        String file = osgEarth::Util::Stringify() << "vec_w/" << z << "/" << y << "/" << x << ext;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            int s = Math::RangeRandom(0, 8);
            String location = template_tdt_url_vec;
            osgEarth::Util::replaceIn(location, "{s}", osgEarth::Util::Stringify() << s);
            osgEarth::Util::replaceIn(location, "{x}", osgEarth::Util::Stringify() << x);
            osgEarth::Util::replaceIn(location, "{y}", osgEarth::Util::Stringify() << y);
            osgEarth::Util::replaceIn(location, "{z}", osgEarth::Util::Stringify() << z);
            location += "&tk=" + mToken + "&f=" + ext;
            if (!downloadFile(location, filename, mTDTHeaders, &mTDTHttpClient))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getTDT_cva_w(String x, String y, String z)
    {
        String ext = ".png";
        String file = osgEarth::Util::Stringify() << "cva_w/" << z << "/" << y << "/" << x << ext;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            int s = Math::RangeRandom(0, 8);
            String location = template_tdt_url_cva;
            osgEarth::Util::replaceIn(location, "{s}", osgEarth::Util::Stringify() << s);
            osgEarth::Util::replaceIn(location, "{x}", osgEarth::Util::Stringify() << x);
            osgEarth::Util::replaceIn(location, "{y}", osgEarth::Util::Stringify() << y);
            osgEarth::Util::replaceIn(location, "{z}", osgEarth::Util::Stringify() << z);
            location += "&tk=" + mToken + "&f=" + ext;
            if (!downloadFile(location, filename, mTDTHeaders, &mTDTHttpClient))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getTDT_ter_w(String x, String y, String z)
    {
        String ext = ".jpg";
        String file = osgEarth::Util::Stringify() << "ter_w/" << z << "/" << y << "/" << x << ext;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            int s = Math::RangeRandom(0, 8);
            String location = template_tdt_url_ter;
            osgEarth::Util::replaceIn(location, "{s}", osgEarth::Util::Stringify() << s);
            osgEarth::Util::replaceIn(location, "{x}", x);
            osgEarth::Util::replaceIn(location, "{y}", y);
            osgEarth::Util::replaceIn(location, "{z}", z);
            location += "&tk=" + mToken + "&f=" + ext;
            if (!downloadFile(location, filename, mTDTHeaders, &mTDTHttpClient))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getGoogle(String lyrs, String x, String y, String z)
    {
        String ext = ".jpg";

        StringVector sv = StringUtil::split(lyrs,"@");
        if (sv.size() > 1)
        {
            lyrs = sv[0];
        }

        if (lyrs == "h")
        {
            ext = ".png";
        }

        String file = osgEarth::Util::Stringify() << "google/" << z << "/" << y << "/" << x << ext;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            String location = template_google_url;
            osgEarth::Util::replaceIn(location, "{l}", lyrs);
            osgEarth::Util::replaceIn(location, "{x}", x);
            osgEarth::Util::replaceIn(location, "{y}", y);
            osgEarth::Util::replaceIn(location, "{z}", z);
            if (!downloadFile(location, filename))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }

    String MapCacher::getTDT_cta_w(String x, String y, String z)
    {
        String ext = ".png";
        String file = osgEarth::Util::Stringify() << "cta_w/" << z << "/" << y << "/" << x << ext;
        String filename = getInCachers(file);
        if (!FileUtil::FileExist(filename))
        {
            int s = Math::RangeRandom(0, 8);
            String location = template_tdt_url_cta;
            osgEarth::Util::replaceIn(location, "{s}", osgEarth::Util::Stringify() << s);
            osgEarth::Util::replaceIn(location, "{x}", osgEarth::Util::Stringify() << x);
            osgEarth::Util::replaceIn(location, "{y}", osgEarth::Util::Stringify() << y);
            osgEarth::Util::replaceIn(location, "{z}", osgEarth::Util::Stringify() << z);
            location += "&tk=" + mToken + "&f=" + ext;
            if (!downloadFile(location, filename, mTDTHeaders, &mTDTHttpClient))
            {
                filename = StringUtil::BLANK;
            }
        }

        return filename;
    }
}