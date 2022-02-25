#ifndef _MAP_CACHER_H__
#define _MAP_CACHER_H__

#include "HttpServerDefine.h"
#include "OCMain/osg.h"
#include "OCMain/osgearth.h"
#include "OCMain/Environment.h"
#include "OCHttpClient/HttpClient.h"

namespace OC
{
    class CeConfigHandler;
    class _HttpServerExport MapCacher
    {
    public:
        MapCacher();
        ~MapCacher();

        static MapCacher* getSingletonPtr();
        static MapCacher& getSingleton();

        void getTdt();

        String getCesium_terrain(String x, String y, String z);

        String getTDT_elv_c(String x, String y, String z);

        String getTDT_img_w(String x, String y, String z);
        String getTDT_cia_w(String x, String y, String z);

        String getTDT_vec_w(String x, String y, String z);
        String getTDT_cva_w(String x, String y, String z);

        String getTDT_ter_w(String x, String y, String z);
        String getTDT_cta_w(String x, String y, String z);

        String getGoogle(String lyrs,String x, String y, String z);
        String getInCachers(String lastfix);
    private:
        bool downloadFile(String url, String filename, StringMap sm = StringMap(),OC::HTTPClient* client = NULL);
        time_t                                  mLastTime;
        String                                  mToken;
        String                                  mAccessToken;
        StringVector                            mCacheRoot;
        StringMap                               mTDTHeaders;
        StringMap                               mCesiumHeaders;
        OC::HTTPClient                          mTDTHttpClient;
        OC::HTTPClient                          mCesiumHttpClient;
    };
}


#endif // !_MAP_CACHER_H__

