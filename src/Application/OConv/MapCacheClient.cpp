#include "ConvCommand.h"
#include "curl/curl.h"
#include "OCHttpServer/MapCacher.h"
#include "Thread/ConvWorkqueue.h"

int CacheStartLevel = 5;
int CacheEndLevel = 13; 
bool isCesium = false;
String tdt_ter = "http://127.0.0.1:8082/OCMapCacher/DataServer?T=elv_c&tk=ef6151d9f0386f3b2a2fdf1d58fe9b32&x={x}&y={y}&l={z}";
String tdt_url = "http://127.0.0.1:8082/OCMapCacher/{m}/wmts?&TileMatrix={z}&TileRow={y}&TileCol={x}";
String gg_url = "http://127.0.0.1:8082/OCMapCacher/vt?lyrs={m}&hl=zh-CN&gl=cn&x={x}&y={y}&z={z}&s=Galil";
String template_cesium_url_ter = "http://127.0.0.1:8125/OCmapcacher/cesium/terrain?l={z}&x={y}&y={x}&v=1.2.0&terrain%2F=undefined";
String template_cesium_url_ter1 = "http://10.104.207.155:8125/OCmapcacher/cesium/terrain?l={z}&x={y}&y={x}&v=1.2.0&terrain%2F=undefined";

struct CacheGrid
{
    CacheGrid(int _z, int _y, int _x) :x(_x), y(_y), z(_z) {}
    int x, y, z;
};


inline void fakeRequest(String url, String map, CacheGrid grid, int level)
{
    int x = grid.x;
    int y = grid.y;
    int z = grid.z;
    int d = level - z;
    int n = pow(2, d);
    int c0 = x * n;
    int c1 = c0 + n;
    int r0 = y * n;
    int r1 = r0 + n;
    uint32 cout = 0;

    float size = (c1 - c0) * (r1 - r0);
    for (int c = c0; c < c1; c++)
    {
        for (int r = r0; r < r1; r++)
        {
            int l1 = isCesium ? level - 1 : level;
            int x1 = isCesium ? r : c;
            int y1 = isCesium ? c : r;

            String urlXYZ = StringUtil::replaceAll(url, "{z}", StringConverter::toString(l1));
            urlXYZ = StringUtil::replaceAll(urlXYZ, "{y}", StringConverter::toString(y1));
            urlXYZ = StringUtil::replaceAll(urlXYZ, "{x}", StringConverter::toString(x1));
            urlXYZ = StringUtil::replaceAll(urlXYZ, "{m}", map);
            int percent = ++cout / size * 100.0;
            osgEarth::HTTPRequest req(urlXYZ);
            osgEarth::HTTPResponse response = osgEarth::HTTPClient::get(req);
            if (!response.isOK())
            {
#if 0
                unsigned int part_num = response.getNumParts() > 1 ? 1 : 0;
                std::istream& input_stream = response.getPartStream(part_num);
                input_stream.seekg(0, std::ios::end);
                int length = input_stream.tellg();
                input_stream.seekg(0, std::ios::beg);
                String str;
                str.resize(length);
                input_stream.read(&str[0], length);
                std::cout << str;
#endif
                std::cout << "fail :";
            }
            std::cout << "[" << z << " " << y << " " << x << "]" << map << "_" << l1 << "_" << y1 << "_" << x1 << "[" << percent << "%]" << std::endl;
        }
    }
}

inline void cacheGrid(String url, String map, CacheGrid grid)
{
    for (int z = (std::max)(CacheStartLevel,grid.z); z < CacheEndLevel + 1; z++)
    {
        fakeRequest(url, map, grid, z);
    }
}

typedef std::vector<CacheGrid> CacheGridList;

void mapcachingCesium(CacheGridList list)
{
    StringVector sv;
    sv.push_back("terrain");
    String url = template_cesium_url_ter;
    for (auto map : sv)
    {
        for (auto g : list)
        {
            cacheGrid(url, map, g);
        }
    }
}

void mapcachingTDT(CacheGridList list)
{
    StringVector sv;
    sv.push_back("img_w");
    sv.push_back("cia_w");
    sv.push_back("vec_w");
    sv.push_back("cva_w");
    sv.push_back("ter_w");
    sv.push_back("cta_w");
    String url = tdt_url;
    for (auto map : sv)
    {
        for (auto g : list)
        {
            cacheGrid(url, map, g);
        }
    }
}

OConvWorkqueue::TaskRequest pushCes(int x, int y, int z, int level)
{
    OConvWorkqueue::TaskRequest rq;
    rq.type = OConvWorkqueue::RT_MapCacherCes;
    rq.X = x;
    rq.Y = y;
    rq.Z = z;
    rq.L = level;
    return rq;
}

OConvWorkqueue::TaskRequest pushTdt(String map,int x,int y,int z,int level)
{
    OConvWorkqueue::TaskRequest rq;
    rq.type = OConvWorkqueue::RT_MapCacherTDT;
    rq.param = map;
    rq.X = x;
    rq.Y = y;
    rq.Z = z;
    rq.L = level;
    return rq;
}

void mapcaching(OC::Json::Value& root)
{
    int threadn = JsonPackage::getInt(root, "Thread", 1);
    OConvWorkqueue workqueue(threadn);

    int x0 = JsonPackage::getInt(root, "StartX", -1);
    int x1 = JsonPackage::getInt(root, "EndX", -1);
    int y0 = JsonPackage::getInt(root, "StartY", -1);
    int y1 = JsonPackage::getInt(root, "EndY", -1);
    int z0 = JsonPackage::getInt(root, "StartZ", 0);
    int z1 = JsonPackage::getInt(root, "EndZ", 0);
    int kz = JsonPackage::getInt(root, "SkipZ", 18);

    String map = JsonPackage::getString(root, "map");
    if (map == "Cesium")
    {
        for (int z = z0; z <= z1; z++)
        {
            if (z <= kz)
            {
                continue;
            }
            for (int y = y0; y <= y1; y++)
            {
                for (int x = x0; x <= x1; x++)
                {
                    OConvWorkqueue::TaskRequest rq = pushCes(x, y, z0, z);
                    workqueue.addRequest(rq);
                }
            }
        }
    }
    else
    {
        StringVector sv;
        sv.push_back("img_w");
        sv.push_back("cia_w");
        sv.push_back("vec_w");
        sv.push_back("cva_w");
        sv.push_back("ter_w");
        sv.push_back("cta_w");
        String url = tdt_url;
        for (auto map : sv)
        {
            for (int z = z0; z <= z1; z++)
            {
                if (z <= kz)
                {
                    continue;
                }
                for (int y = y0; y <= y1; y++)
                {
                    for (int x = x0; x <= x1; x++)
                    {
                        OConvWorkqueue::TaskRequest rq = pushTdt(map, x, y, z0, z);
                        workqueue.addRequest(rq);
                    }
                }
            }
        }
    }

    while (workqueue.taskCount > 0)
    {
        Sleep(1000);
        workqueue.mWorkQueue->processResponses();
    }

    return;

#if 0
    osgEarth::HTTPClient::setTimeout(10);

    CacheStartLevel = JsonPackage::getInt(root, "CacheStartLevel", 0);
    CacheEndLevel = JsonPackage::getInt(root, "CacheEndLevel", 13);

    int z1 = JsonPackage::getInt(root, "z1", -1);
    int x1 = JsonPackage::getInt(root, "x1", -1);
    int x2 = JsonPackage::getInt(root, "x2", -1);
    int y1 = JsonPackage::getInt(root, "y1", -1);
    int y2 = JsonPackage::getInt(root, "y2", -1);

    if (z1 == -1 || x1 == -1 || x2 == -1 || y1 == -1 || y2 == -1)
    {
        return;
    }

    CacheGridList list;
    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            list.push_back(CacheGrid(z1, y, x));
        }
    }
    String map = JsonPackage::getString(root, "map");
    if (map == "Cesium")
    {
        isCesium = true;
        mapcachingCesium(list);
    }
    else
    {
        mapcachingTDT(list);
    }
#endif
}
