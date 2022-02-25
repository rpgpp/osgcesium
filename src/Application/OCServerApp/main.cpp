// OCServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "OCMain/ConfigManager.h"
#include "OCHttpServer/HttpServer.h"

using namespace OC;

HttpServer* server = NULL;

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType) {

    if (CTRL_CLOSE_EVENT == dwCtrlType) {
        CORE_SAFE_DELETE(server);
    }

    return TRUE;
}

int main(int argc, char** argv)
{
    SetConsoleCtrlHandler(HandlerRoutine,true);
    osg::ArgumentParser arguments(&argc, argv);
    int port = 8125;
    while (arguments.read("--port", port)) {}
    server = new HttpServer;
    server->init(port);
    return 0;
}

