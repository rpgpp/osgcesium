#pragma warning(disable:4005)
#include <drogon/HttpController.h>
#include "OCMain/Environment.h"
#include "OCMain/ConfigManager.h"
#include "OCHttpClient/HttpClient.h"

using namespace drogon;

class weaService : public HttpController<weaService>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(weaService::genericHome, "", Get);
    METHOD_ADD(weaService::genericHome, "MusicProxy/api", Get);
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

    void genericHome(const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        const OC::String hostUrl = Singleton(OC::ConfigManager).getStringValue("TianQingHost");
        auto resp = HttpResponse::newHttpResponse();
        const OC::String query = req->getQuery();
        if (query.empty() || hostUrl.empty())
        {
            resp->setStatusCode(k404NotFound);
        }
        else
        {
            OC::String rawUrl = hostUrl + "/music-ws/api?" + query;
            OC::HTTPRequest request(rawUrl);
            osgEarth::ReadResult result = OC::HTTPClient::readString(request);
            if (result.succeeded())
            {
                resp->setContentTypeCode(CT_APPLICATION_JSON);
                OC::String s = result.getString();
                resp->setBody(s);
            }
            else
            {
                resp->setStatusCode(k404NotFound);
            }
        }



        callback(resp);
    }
};