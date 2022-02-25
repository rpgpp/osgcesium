#pragma once

#include <qthread.h>
#include "OCHttpServer/HttpServer.h"

class ServerThread : public QThread
{
public:
    ServerThread(int port, OC::String path = ""):_port(port), _path(path){}
    OC::HttpServer& server(){return mServer;}
private:
    void run()
    {
        mServer.init(_port);
    }

    OC::HttpServer mServer;
    int _port;
    OC::String _path;
};