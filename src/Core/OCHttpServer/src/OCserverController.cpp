#pragma warning(disable:4005)
#include <drogon/HttpController.h>
#include "OCServer/OCServer.h"
#include "HttpServerDefine.h"

using namespace drogon;

class OCserver : public HttpController<OCserver>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(OCserver::genericHome, "", Get);
    METHOD_ADD(OCserver::genericOC1, "/p/OC", Get);
    METHOD_ADD(OCserver::genericOC2, "/b/OC", Get);
    METHOD_ADD(OCserver::genericOG, "/b/OG", Post);
    METHOD_ADD(OCserver::weathertoy, "/weathertoy", Post);
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

    void genericHome(const HttpRequestPtr &,
                      std::function<void(const HttpResponsePtr &)> &&callback)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("OCServer is running well.");
        callback(resp);
    }

    void genericOC1(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String method = getParam(req, "method");
        OC::String param = getParam(req, "param");
        param = "p;" + param;
        OC::String filter = getParam(req, "filter");
        OC::String filename = Singleton(OC::Server::COCServer).getDataInfo(method, param, filter);
        auto resp = drogon::HttpResponse::newFileResponse(filename, osgDB::getSimpleFileName(filename));
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "max-age=604800");
        callback(resp);
    }

    void genericOC2(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String method = getParam(req, "method");
        OC::String param = getParam(req, "param");
        param = "b;" + param;
        OC::String filter = getParam(req, "filter");
        OC::MemoryDataStreamPtr dataStream = Singleton(OC::Server::COCServer).getByteData(method, param, filter);
        if (!dataStream.isNull())
        {
            auto resp = drogon::HttpResponse::newFileResponse(dataStream->getPtr(), dataStream->size(), dataStream->getName());
            callback(resp);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Cache-Control", "max-age=604800");
        }
        else
        {
            auto resp = drogon::HttpResponse::newNotFoundResponse();
            callback(resp);
        }
    }
    
    void genericOG(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback)
    {
        OC::String method = getParam(req, "method");
        OC::String param = "post;" + getParam(req, "param");
        OC::String filter(req->bodyData(), req->bodyLength());
        
        OC::MemoryDataStreamPtr dataStream = Singleton(OC::Server::COCServer).getByteData(method, param, filter);
        if (!dataStream.isNull())
        {
            auto resp = drogon::HttpResponse::newFileResponse(dataStream->getPtr(), dataStream->size(), dataStream->getName());
            callback(resp);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Cache-Control", "max-age=604800");
        }
        else
        {
            auto resp = drogon::HttpResponse::newNotFoundResponse();
            callback(resp);
        }
    }

    void weathertoy(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        OC::String filename(req->bodyData(), req->bodyLength());

        OC::MemoryDataStreamPtr dataStream = Singleton(OC::Server::COCServer).getByteData(filename);
        if (!dataStream.isNull())
        {
            auto resp = drogon::HttpResponse::newFileResponse(dataStream->getPtr(), dataStream->size(), dataStream->getName());
            callback(resp);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Cache-Control", "max-age=604800");
        }
        else
        {
            auto resp = drogon::HttpResponse::newNotFoundResponse();
            callback(resp);
        }
    }
};