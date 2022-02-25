#ifndef _OC_HTTP_SERVER_H_
#define _OC_HTTP_SERVER_H_

#include "HttpServerDefine.h"

namespace OC
{
    namespace Server {
        class COCServer;
    }
    class _HttpServerExport HttpServer
    {
    public:
        HttpServer();
        ~HttpServer();
        bool isRunning();
        void quit();
        void init(int port = 8125);
        void registerALocation(std::string uriPrefix, std::string path);

    private:
        Server::COCServer* mOCServer;
    };
}

#endif // !1
