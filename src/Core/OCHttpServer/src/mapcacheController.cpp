#pragma warning(disable:4005)
#include <drogon/HttpController.h>
#include "MapCacher.h"

using namespace drogon;

class OCMapCacher : public HttpController<OCMapCacher>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(OCMapCacher::genericHome, "", Get);
    METHOD_ADD(OCMapCacher::genericOCimg, "/img_w/wmts", Get);
    METHOD_ADD(OCMapCacher::genericOCcia, "/cia_w/wmts", Get);
    METHOD_ADD(OCMapCacher::genericOCvec, "/vec_w/wmts", Get);
    METHOD_ADD(OCMapCacher::genericOCcva, "/cva_w/wmts", Get);
    METHOD_ADD(OCMapCacher::genericOCter, "/ter_w/wmts", Get);
    METHOD_ADD(OCMapCacher::genericOCcta, "/cta_w/wmts", Get);
    METHOD_ADD(OCMapCacher::genericOCcta, "/cta_w/wmts", Get);
    METHOD_ADD(OCMapCacher::genericOCvt, "/vt", Get);
    METHOD_ADD(OCMapCacher::genericOCDataServer, "/DataServer", Get);
    METHOD_ADD(OCMapCacher::genericOCessimTerrain, "/cesium/terrain", Get);
    METHOD_ADD(OCMapCacher::genericOCessimTerrain, "/cesium/terrain/layer", Get);
    METHOD_ADD(OCMapCacher::genericOCessimTerrain, "/cesium/layer.json?terrain", Get);
    METHOD_ADD(OCMapCacher::genericOCessimTerrain, "/cesium/layer.json?terrain/", Get);
    METHOD_LIST_END

  protected:
    inline OC::String getParam(const drogon::HttpRequestPtr& req, OC::String paramName)
    {
        OC::String p;
        const std::unordered_map<std::string, std::string>& paramteters = req->parameters();
        std::unordered_map<std::string, std::string>::const_iterator it;
        it = paramteters.find(paramName);
        if (it != paramteters.end())
        {
            p = it->second;
        }
        return p;
    };

    void genericHome(const HttpRequestPtr&,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("OCMapCacher is running well.");
        callback(resp);
    }
    
    void genericOCessimTerrain(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "l");
        OC::String y = getParam(req, "y");
        OC::String x = getParam(req, "x");
        OC::String filename;
        if (z == "" || x == "" || y == "")
        {
            filename = Singleton(OC::Environment).getAppDir() + "config/CesiumTerrain.json";
        }
        else
        {
            filename = Singleton(OC::MapCacher).getCesium_terrain(x, y, z);
        }

        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }

    void genericOCDataServer(const HttpRequestPtr& req,
            std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "l");
        OC::String y = getParam(req, "y");
        OC::String x = getParam(req, "x");
        OC::String filename = Singleton(OC::MapCacher).getTDT_elv_c(x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }

    void genericOCimg(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "TileMatrix");
        OC::String y = getParam(req, "TileRow");
        OC::String x = getParam(req, "TileCol");
        OC::String filename = Singleton(OC::MapCacher).getTDT_img_w(x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }

    void genericOCcia(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "TileMatrix");
        OC::String y = getParam(req, "TileRow");
        OC::String x = getParam(req, "TileCol");
        OC::String filename = Singleton(OC::MapCacher).getTDT_cia_w(x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }

    void genericOCvec(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "TileMatrix");
        OC::String y = getParam(req, "TileRow");
        OC::String x = getParam(req, "TileCol");
        OC::String filename = Singleton(OC::MapCacher).getTDT_vec_w(x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }

    void genericOCcva(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "TileMatrix");
        OC::String y = getParam(req, "TileRow");
        OC::String x = getParam(req, "TileCol");
        OC::String filename = Singleton(OC::MapCacher).getTDT_cva_w(x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }

    void genericOCter(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "TileMatrix");
        OC::String y = getParam(req, "TileRow");
        OC::String x = getParam(req, "TileCol");
        OC::String filename = Singleton(OC::MapCacher).getTDT_ter_w(x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }
    
    void genericOCcta(const HttpRequestPtr& req,
            std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String z = getParam(req, "TileMatrix");
        OC::String y = getParam(req, "TileRow");
        OC::String x = getParam(req, "TileCol");
        OC::String filename = Singleton(OC::MapCacher).getTDT_cta_w(x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }

    void genericOCvt(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String layer = getParam(req, "lyrs");
        OC::String z = getParam(req, "z");
        OC::String y = getParam(req, "y");
        OC::String x = getParam(req, "x");
        OC::String filename = Singleton(OC::MapCacher).getGoogle(layer,x, y, z);
        OC::String name = osgDB::getSimpleFileName(filename);
        auto resp = drogon::HttpResponse::newFileResponse(filename, name, CT_NONE);
        resp->addHeader("Content-Disposition", "");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        resp->addHeader("Connection", "keep-alive");
        callback(resp);
    }
};