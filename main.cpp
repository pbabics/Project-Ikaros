#include "Includes.h"
#include "Utils.cpp"
#include "Shared.h" // Shared Libraries
#include "SignalHandler.h" // Signal Handler
#include "file.h" // AsciiFile, BinarryFile, BinnaryData
#include "ThreadMgr.h" // ThreadMgr
#include "Socket.h" // Socket
#include "SocketMgr.h" // SocketMgr
#include "Handler.h"
#include "SimpleLog.h" // Loging
#include "Application.h"
#include "ConfigMgr.h"

SimpleLog* sLog;
Application* app;

int main(int argc, char* argv[])
{
    app = new Application(argc, argv, "./config.conf");

    return app->Update();
}
