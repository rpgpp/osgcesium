#include "OConvMapCacher.h"
#include "curl/curl.h"
#include "OCMain/DataStream.h"
#include "ConvCommand.h"
#include "OCHttpServer/MapCacher.h"

namespace OC
{
	OConvMapCacher::OConvMapCacher()
	{

	}

	OConvMapCacher::~OConvMapCacher()
	{

	}

    void OConvMapCacher::requestTDT(String map, int x, int y, int z, int level)
    {
        time_t t0 = time(0);
        MapCacher& cacher = Singleton(MapCacher);
        int d = level - z;
        int n = pow(2, d);
        int c0 = x * n;
        int r0 = y * n;
        uint32 cout = 0;
        float size = n * n;
        for (int c = 0; c < n; c++)
        {
            for (int r = 0; r < n; r++)
            {
                int percent = ++cout / size * 100.0;
                String L1 = osgEarth::Stringify() << level;
                String X1 = osgEarth::Stringify() << c0 + c;
                String Y1 = osgEarth::Stringify() << r0 + r;
                String filename;
                if (map == "img_w")
                {
                    filename = cacher.getTDT_img_w(X1, Y1, L1);
                }
                else if (map == "vec_w")
                {
                    filename = cacher.getTDT_vec_w(X1, Y1, L1);
                }
                else if (map == "ter_w")
                {
                    filename = cacher.getTDT_ter_w(X1, Y1, L1);
                }
                else if (map == "cia_w")
                {
                    filename = cacher.getTDT_cia_w(X1, Y1, L1);
                }
                else if (map == "cva_w")
                {
                    filename = cacher.getTDT_cva_w(X1, Y1, L1);
                }
                else if (map == "cta_w")
                {
                    filename = cacher.getTDT_cta_w(X1, Y1, L1);
                }
                
                bool error = !FileUtil::FileExist(filename);
                if (error)
                {
                    OCLogStream << "Error: " << filename;
                }
                time_t t1 = time(0);
                if (error || t1 - t0 > 1)
                {
                    OCLogStream << "[" << z << " " << y << " " << x << "]" << map << "_" << L1 << "_" << Y1 << "_" << X1 << "[" << percent << "%]";
                    t0 = t1;
                }
            }
        }
    }

    void OConvMapCacher::requestCes(int x, int y, int z,int level)
    {
        time_t t0 = time(0);
        MapCacher& cacher = Singleton(MapCacher);
        int d = level - z;
        int n = 1 << d;
        int numy = 1 << level;
        int c0 = x * n;
        int r0 = y * n;
        uint32 cout = 0;
        String map = "terrain";
        float size = n * n;
        for (int c = 0; c < n; c++)
        {
            for (int r = 0; r < n; r++)
            {
                int percent = ++cout / size * 100.0;
                String L1 = osgEarth::Stringify() << level;
                String X1 = osgEarth::Stringify() << c0 + c;
                String Y1 = osgEarth::Stringify() << numy - 1 - (r0 + r);
                String filename = cacher.getCesium_terrain(X1, Y1, L1);
                StringStream info;
                bool error = !FileUtil::FileExist(filename);
                if (error)
                {
                    info << "Error: ";
                }
                info << "[" << z << " " << x << " " << y << "]" << map << "_" << L1 << "_" << X1 << "_" << Y1 << "[" << percent << "%]";
                time_t t1 = time(0);
                if (error || t1 - t0 > 1)
                {
                    OCLogStream << info.str();
                    t0 = t1;
                }
            }
        }
    }
}