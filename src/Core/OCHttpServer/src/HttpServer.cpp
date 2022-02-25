#include "HttpServer.h"
#include <string_view>
#pragma warning(disable:4005)
#include "drogon/drogon.h"
#include "OCMain/Environment.h"
#include "OCMain/osg.h"
#include "OCMain/LogManager.h"
#include "OCServer/OCServer.h"

#ifdef LOG_INFO
#undef LOG_INFO
#endif
#define LOG_INFO                                               \
    if (trantor::Logger::logLevel() <= trantor::Logger::kInfo) \
    trantor::Logger("", 0).stream()

namespace OC
{
    HttpServer::HttpServer()
    {
        const char* banner = "\n\
 ..#######..######..######.#######.########.##.....#.#######.########.\n\
 .##.....#.##....#.##....#.##......##.....#.##.....#.##......##.....##\n\
 .##.....#.##......##......##......##.....#.##.....#.##......##.....##\n\
 .##.....#.##.......######.######..########.##.....#.######..########.\n\
 .##.....#.##............#.##......##...##...##...##.##......##...##..\n\
 .##.....#.##....#.##....#.##......##....##...##.##..##......##....##.\n\
 ..#######..######..######.#######.##.....#....###...#######.##.....##\n";
        std::cout << banner << std::endl;
        mOCServer = new Server::COCServer;
        mOCServer->init();
    }

    HttpServer::~HttpServer()
    {
        quit();
        CORE_SAFE_DELETE(mOCServer);
    }

    void HttpServer::quit()
    {
        drogon::app().quit();
    }

    bool HttpServer::isRunning()
    {
        return drogon::app().isRunning();
    }

    void HttpServer::registerALocation(String uriPrefix, String path)
    {
        drogon::app().addALocation(uriPrefix, "", path);
    }

    void HttpServer::init(int port)
    {
        String workspace = Singleton(Environment).getAppDir();
        String mCacheRoot = Singleton(OC::ConfigManager).getStringValue("MapCacheDir", "d:/map_caches/");
        String filename = workspace + "config.json";
        drogon::app().loadConfigFile(filename);
        //drogon::app().setLogPath(Singleton(Environment).getTempDir());
        drogon::app().setLogLevel(trantor::Logger::kInfo);
        drogon::app().setLogPath("");

        drogon::app().registerHandler(
            "/",
            [](const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setBody("Server is running well!");
            callback(resp);
        },
            { drogon::Get });

        drogon::app().addListener("0.0.0.0", port);

        class HttpLogListener : public LogListener
        {
        public:
            virtual void messageLogged(const String& message, LogMessageLevel lml, bool maskDebug, const String& logName, bool& skipThisMessage)
            {
                skipThisMessage = true;
                LOG_INFO << message;
            }
        };

        Singleton(LogManager).getDefaultLog()->addListener(new HttpLogListener());

        OCLogStream << "MapCaches:" << mCacheRoot;
        OCLogStream << "Load Config:" << filename;
        OCLogStream << "Server running on 0.0.0.0:" << port;
        drogon::app().run();
    }
}