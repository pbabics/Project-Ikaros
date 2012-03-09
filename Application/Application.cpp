#include "Application.h"


Application::Application(int argc, char* argv[], const char *conf):
debug(false), control(false), daemonize(false), libLoaded(false), terminate(false)
{
    if (argc > 0)
    {
        ApplicationAddress = argv[0];
        for (int i = 1 ; i < argc; i++)
        {
            string arg = string(argv[i]);
            string property, value;
            refTrim(arg);
            if (arg.find('=') != string::npos)
            {
                property = arg.substr(arg.find_first_not_of('-'), arg.find('=') - arg.find_first_not_of('-'));
                refTrim(property);
                value =  arg.substr(arg.find('=') + 1 , string::npos);
                refTrim(value);
            }
            else
            {
                property = arg.substr(arg.find_first_not_of('-'), string::npos);
                value = string("1");
            }
            RunOptions.insert(make_pair(property, value));
        }
    }

    if (conf)
    {
        AsciiFile config(conf);
        if (config)
        {
            char buff[1024];
            string line ;
            do
            {
                config.getline(buff, 1024);
                line = buff;
                if( (line.length() > 0) &&
                    (line[0] != '#') && (line[0] != ';') &&
                    (line.find('=') != string::npos))
                {
                    refTrim(line);
                    string property = line.substr(0, line.find('='));
                    refTrim(property);
                    string value =  line.substr(line.find('=') + 1, string::npos);
                    refTrim(value);
                    if (property.length() && value.length())
                        FileOptions.insert(make_pair(property, value));
                }
            } while (!config.eof());
        }
    }
    ParseParams();

    if (BoolConfigs[CONFIG_BOOL_DAEMONIZE])
        Daemonize(StringConfigs[CONFIG_STRING_PID_FILE].c_str(), StringConfigs[CONFIG_STRING_WORKING_DIRECTORY].c_str());

    _initGlobals();
}

void Application::ParseParams()
{
    LoadConfigs();
    switch (IntConfigs[CONFIG_INT_LOG_LEVEL])
    {
        case 3:
        case 2: debug = true;
        case 1: control = true;
    }

}

void Application::LoadConfigs()
{
    for (register int i = 0; i < CONFIG_INT_MAX; ++i)
        IntConfigs[i] = 0;

    for (register int i = 0; i < CONFIG_BOOL_MAX; ++i)
        BoolConfigs[i] = false;

    for (register int i = 0; i < CONFIG_STRING_MAX; ++i)
        StringConfigs[i] = string("");

    if (FileOptions.size() > 0)
    {
        LoadBoolConfig("Server.Daemonize", CONFIG_BOOL_DAEMONIZE, false);
        LoadStringConfig("Server.PidFile", CONFIG_STRING_PID_FILE, "/var/lock/ikaros.pid");
        LoadStringConfig("Server.WorkingDirectory", CONFIG_STRING_WORKING_DIRECTORY, ApplicationAddress.substr(0, ApplicationAddress.find_last_of('/')));
        LoadBoolConfig("Server.Log", CONFIG_BOOL_LOG, true);
        LoadIntConfig("Server.LogLevel", CONFIG_INT_LOG_LEVEL, 0);

        LoadIntConfig("Protocol.BindPort", CONFIG_INT_BIND_PORT, 3535);
        LoadStringConfig("Protocol.BindIp", CONFIG_STRING_BIND_IP, "127.0.0.1");
        LoadStringConfig("Protocol.ProtocolName", CONFIG_STRING_PROTOCOL_NAME);
        LoadStringConfig("Protocol.LibraryPath", CONFIG_STRING_LIBRARY_PATH);
        LoadStringConfig("Protocol.RecvFunc", CONFIG_STRING_RECV_FUNC);
        LoadStringConfig("Protocol.SendFunc", CONFIG_STRING_SEND_FUNC);
        LoadStringConfig("Protocol.ConnectFunc", CONFIG_STRING_CONNECT_FUNC);
        LoadStringConfig("Protocol.DisconnectFunc", CONFIG_STRING_DISCONNECT_FUNC);
        LoadStringConfig("Protocol.LoadFunc", CONFIG_STRING_LOAD_FUNC);
        LoadStringConfig("Protocol.UnloadFunc", CONFIG_STRING_UNLOAD_FUNC);
    }

    if (RunOptions.size() > 0)
    {
        LoadBoolRunConfig("daemonize", "d", CONFIG_BOOL_DAEMONIZE);
        LoadBoolRunConfig("log", "l", CONFIG_BOOL_LOG);
        LoadIntRunConfig("loglevel", "ll", CONFIG_INT_LOG_LEVEL);
    }
}

void Application::outDebugParams() const
{
    if (isDaemon())
    {
        sLog->outString("****** Runnig as Daemon ******");
        sLog->outString("Process Id:  %d  Parent Process Id: %d  Process Group Id: %d",
        GetProcessId(), GetParentProcessId(), GetCurrentProcessGroupId());
        sLog->outString("Working Directory %s", GetWorkingDirectory());
        sLog->outString();
    }

    sLog->outString("CONFIG_BOOL_DAEMONIZE=%s", toString(BoolConfigs[CONFIG_BOOL_DAEMONIZE]));
    sLog->outString("CONFIG_STRING_PID_FILE='%s'", StringConfigs[CONFIG_STRING_PID_FILE].c_str());
    sLog->outString("CONFIG_STRING_WORKING_DIRECTORY='%s'", StringConfigs[CONFIG_STRING_WORKING_DIRECTORY].c_str());
    sLog->outString("CONFIG_BOOL_LOG=%s", toString(BoolConfigs[CONFIG_BOOL_LOG]));
    sLog->outString("CONFIG_INT_LOG_LEVEL=%d", IntConfigs[CONFIG_INT_LOG_LEVEL]);
    sLog->outString("Control=%s", toString(control));
    sLog->outString("Debug=%s", toString(debug));
    sLog->outString();
    sLog->outString("CONFIG_INT_BIND_PORT=%d", IntConfigs[CONFIG_INT_BIND_PORT]);
    sLog->outString("CONFIG_STRING_BIND_IP='%s'", StringConfigs[CONFIG_STRING_BIND_IP].c_str());
    sLog->outString("CONFIG_STRING_PROTOCOL_NAME='%s'", StringConfigs[CONFIG_STRING_PROTOCOL_NAME].c_str());
    sLog->outString("CONFIG_STRING_LIBRARY_PATH='%s'", StringConfigs[CONFIG_STRING_LIBRARY_PATH].c_str());
    sLog->outString("CONFIG_STRING_RECV_FUNC='%s'", StringConfigs[CONFIG_STRING_RECV_FUNC].c_str());
    sLog->outString("CONFIG_STRING_SEND_FUNC='%s'", StringConfigs[CONFIG_STRING_SEND_FUNC].c_str());
    sLog->outString("CONFIG_STRING_CONNECT_FUNC='%s'", StringConfigs[CONFIG_STRING_CONNECT_FUNC].c_str());
    sLog->outString("CONFIG_STRING_DISCONNECT_FUNC='%s'", StringConfigs[CONFIG_STRING_DISCONNECT_FUNC].c_str());
    sLog->outString("CONFIG_STRING_LOAD_FUNC='%s'", StringConfigs[CONFIG_STRING_LOAD_FUNC].c_str());
    sLog->outString("CONFIG_STRING_UNLOAD_FUNC='%s'", StringConfigs[CONFIG_STRING_UNLOAD_FUNC].c_str());
}

void Application::outDebugLibrary() const
{
    sLog->outString();
    sLog->outString("Library Name: %s", libLoader->GetLibraryPath());
    sLog->outString("Is Open: %s", toString(libLoader->is_open()));
    sLog->outString("Is Loaded: %s", toString(libLoaded));
    sLog->outString("Recieve Function at Address: 0x%08x", proto.OnRecieve);
    sLog->outString("Send Function at Address: 0x%08x", proto.OnSend);
    sLog->outString("Connect Function at Address: 0x%08x", proto.OnConnect);
    sLog->outString("Disconnect Function at Address: 0x%08x", proto.OnDisconnect);
    sLog->outString("Load Function at Address: 0x%08x", proto.OnLoad);
    sLog->outString("Unload Function at Address: 0x%08x", proto.OnUnload);
    sLog->outString();
}

void Application::LoadIntConfig(string ConfigName, uint32 Config, uint32 Default)
{
    if (FileOptions.size() > 0)
    {
        if (FileOptions.find(ConfigName) != FileOptions.end())
        {
            stringstream s;
            s.str(FileOptions[ConfigName]);
            s >> IntConfigs[Config];
        }
        else
            IntConfigs[Config] = Default;
    }
}

void Application::LoadStringConfig(string ConfigName, uint32 Config, string Default)
{
    if (FileOptions.size() > 0)
        StringConfigs[Config] = (FileOptions.find(ConfigName) != FileOptions.end() ? FileOptions[ConfigName] : Default);
}

void Application::LoadBoolConfig(string ConfigName, uint32 Config, bool Default)
{
    if (FileOptions.size() > 0)
    {
        if (FileOptions.find(ConfigName) != FileOptions.end())
        {
            stringstream s;
            s.str(FileOptions[ConfigName]);
            s >> BoolConfigs[Config];
        }
        else
            BoolConfigs[Config] = Default;
    }
}

void Application::LoadIntRunConfig(string ConfigName, string ShortFormat, uint32 Config)
{
    if (RunOptions.size() > 0)
    {
        if (RunOptions.find(ConfigName) != RunOptions.end())
        {
            stringstream s;
            s.str(RunOptions[ConfigName]);
            s >> IntConfigs[Config];
        }
        else
        if (RunOptions.find(ShortFormat) != RunOptions.end())
        {
            stringstream s;
            s.str(RunOptions[ShortFormat]);
            s >> IntConfigs[Config];
        }
    }
}

void Application::LoadStringRunConfig(string ConfigName, string ShortFormat, uint32 Config)
{
    if (RunOptions.size() > 0)
    {
        if (RunOptions.find(ConfigName) != RunOptions.end())
            StringConfigs[Config] = RunOptions[ConfigName];
        else
            if (RunOptions.find(ShortFormat) != RunOptions.end())
                StringConfigs[Config] = RunOptions[ShortFormat];
    }
}

void Application::LoadBoolRunConfig(string ConfigName, string ShortFormat, uint32 Config)
{
    if (RunOptions.size() > 0)
    {
        if (RunOptions.find(ConfigName) != RunOptions.end())
        {
            stringstream s;
            s.str(RunOptions[ConfigName]);
            s >> BoolConfigs[Config];
        }
        else
        if (RunOptions.find(ShortFormat) != RunOptions.end())
        {
            stringstream s;
            s.str(RunOptions[ShortFormat]);
            s >> BoolConfigs[Config];
        }
    }
}

bool Application::LoadLibrary()
{
    if (!StringConfigs[CONFIG_STRING_LIBRARY_PATH].length())
        return libLoaded = false;

    if (!libLoader->open(StringConfigs[CONFIG_STRING_LIBRARY_PATH].c_str()))
        return libLoaded = false;

    if (StringConfigs[CONFIG_STRING_SEND_FUNC].length())
        proto.OnSend = onSend(libLoader->findFunc(StringConfigs[CONFIG_STRING_SEND_FUNC].c_str())) ;

    if (StringConfigs[CONFIG_STRING_RECV_FUNC].length())
        proto.OnRecieve = onRecieve(libLoader->findFunc(StringConfigs[CONFIG_STRING_RECV_FUNC].c_str())) ;

    if (StringConfigs[CONFIG_STRING_CONNECT_FUNC].length())
        proto.OnConnect = onConnect(libLoader->findFunc(StringConfigs[CONFIG_STRING_CONNECT_FUNC].c_str()));

    if (StringConfigs[CONFIG_STRING_DISCONNECT_FUNC].length())
        proto.OnDisconnect = onDisconnect(libLoader->findFunc(StringConfigs[CONFIG_STRING_DISCONNECT_FUNC].c_str()));

    if (StringConfigs[CONFIG_STRING_LOAD_FUNC].length())
        proto.OnLoad = onLoad(libLoader->findFunc(StringConfigs[CONFIG_STRING_LOAD_FUNC].c_str()));

    if (StringConfigs[CONFIG_STRING_UNLOAD_FUNC].length())
        proto.OnUnload = onUnload(libLoader->findFunc(StringConfigs[CONFIG_STRING_UNLOAD_FUNC].c_str()));

    return libLoaded = true;
}


void Application::_initGlobals()
{
    sLog = new SimpleLog(control, debug, "Server.log");
    sigHandler = new SignalHandler();
    libLoader = new SharedLibrary();
    socketMgr = new ConnectionMgr();
    threadMgr = new ThreadMgr();
    handler = new PacketHandler();
}

void Application::_uninitGlobals()
{
    delete sigHandler;
    delete libLoader;
    delete socketMgr;
    delete threadMgr;
    delete handler;
    delete sLog;
}

void Application::_InitServerSocket()
{
    int r;
    struct sockaddr_in dest;
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0) ;
    setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, &ServerSocket, sizeof ServerSocket);
    setNonblocking(ServerSocket);
    if (ServerSocket < 0)
    {
        sLog->outError("Socket Creation failed. Error: %d",errno);
        exit(-1);
    }

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(IntConfigs[CONFIG_INT_BIND_PORT]);
    dest.sin_addr.s_addr = inet_addr(StringConfigs[CONFIG_STRING_BIND_IP].c_str());
    //Bind
    r= bind(ServerSocket, (struct sockaddr*)&dest, sizeof(dest));
    if (r == -1)
    {
        sLog->outError("Bind Returned Error: %d", errno);
        close(ServerSocket);
        exit(-1);
    }

    //Listen
    r=listen(ServerSocket, 5);
    if (r == -1)
    {
        sLog->outError("Listen Returned Error: %d", errno);
        close(ServerSocket);
        exit(-1);
    }
    else
        sLog->outControl("[Init Server Socket] Server Socket Listening on: %s:%d",StringConfigs[CONFIG_STRING_BIND_IP].c_str(),IntConfigs[CONFIG_INT_BIND_PORT]);
}

void* CallProcessQueue(void* obj)
{
    ((PacketHandler*)obj)->ProcessQueue();
    return NULL;
}

void HandleInterupt(int /* p */)
{
    app->Terminate();
}

void Application::Terminate()
{
    handler->Terminate();
    if (threadMgr->GetThreadStatus("Queue") == THREAD_SUSPENDED)
        threadMgr->ResumeThread("Queue");
    terminate = true;
}

uint32 Application::Update()
{
    LoadLibrary();
    if (libLoader->getError())
    {
        sLog->outError("Library Loader Returned Error: %s", libLoader->getError());
        return -1;
    }

    if (proto.OnLoad) // Enable Library to Init its variables
        proto.OnLoad();

    outDebugParams();
    outDebugLibrary();
    _InitServerSocket();

    sigHandler->setSignalHandler(Interupt, &HandleInterupt);
    sigHandler->setSignalHandler(SignalType(SIGTERM), &HandleInterupt);

    fd_set Recv, test;
    int nfds = ServerSocket+1 ,r =0;

    FD_ZERO(&Recv);
    FD_SET(ServerSocket, &Recv);

    timeval selectTimeout;
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 500000;

    int ProcessQueue = threadMgr->CreateThread("Queue", &CallProcessQueue, handler);
    if (ProcessQueue == -1)
    {
        sLog->outError("[Recv Thread] Executing Packet Handling Queue Failed errno: %d",errno);
        exit(-1);
    }

    while (!terminate)
    {
        test = Recv;
        selectTimeout.tv_sec = 1;
        selectTimeout.tv_usec = 500000;
        nfds = ((socketMgr->GetHighestFd() > ServerSocket) ? socketMgr->GetHighestFd() : ServerSocket) + 1;
        r = select(nfds, &test, NULL, NULL, &selectTimeout);
        if (r == 0)
            continue;

        if (FD_ISSET(ServerSocket, &test))
        {
            struct sockaddr_in peer;
            unsigned int peerSize = sizeof(peer);
            int newSocket = accept(ServerSocket, (sockaddr*)&peer, &peerSize);
            r--;
            if (newSocket == -1)
            {
                if (errno != EAGAIN)
                    sLog->outError("[Recv Thread] Accept Error: %d", errno);
                continue;
            }
            setNonblocking(newSocket);
            Socket* sock = new Socket(newSocket, peer);
            socketMgr->AddSocket(sock);
            sLog->outDebug("[Recv Thread] Recieved New Connection From: %s FD: %d", inet_ntoa(peer.sin_addr), sock->GetFD());
            if (proto.OnConnect)
            {
                //handler->AddDelayedEvent(DelayedEvent(EVENT_CONNECT, newSocket, peer.sin_addr, 100));
                handler->AddEvent(Event(EVENT_CONNECT, newSocket, peer.sin_addr));
                if (threadMgr->GetThreadStatus(ProcessQueue) == THREAD_SUSPENDED)
                    sLog->outDebug("[Recv Thread] Resuming Queue thread: %d Status: %s", ProcessQueue, runStatus(threadMgr->ResumeThread(ProcessQueue)));
            }
            FD_SET(newSocket, &Recv);
        }

        if (r > 0)
            for (ConnectionMgr::SocketMap::iterator sock = socketMgr->sockets.begin(); sock != socketMgr->sockets.end() && r; sock++)
                if (FD_ISSET(sock->second->GetFD(), &test))
                {
                    BinnaryData* data = new BinnaryData();
                    data->reserve(16 * IN_KILOBYTES);
                    int recieved = sock->second->Recieve(*data);

                    if (recieved == 0)
                    {
                        sLog->outDebug("[Recv Thread] Client from %s disconnected (fd: %d)", sock->second->GetAddress(), sock->first);
                        socketMgr->CloseSocketByFd(sock->second->GetFD());
                        FD_CLR(sock->second->GetFD(), &Recv);
                        if (proto.OnDisconnect)
                        {
                            handler->AddEvent(Event(EVENT_DISCONNECT, sock->first, sock->second->GetAddr().sin_addr));
                            if (threadMgr->GetThreadStatus(ProcessQueue) == THREAD_SUSPENDED)
                                sLog->outDebug("[Recv Thread] Resuming Queue thread: %d Status: %s", ProcessQueue, runStatus(threadMgr->ResumeThread(ProcessQueue)));
                        }
                    }
                    else
                        if (recieved > 0)
                        {
                            if (proto.OnRecieve)
                            {
                                handler->AddEvent(Event(EVENT_RECIEVE, sock->first, sock->second->GetAddr().sin_addr, *data, recieved, sock->second->errnum()));
                                if (threadMgr->GetThreadStatus(ProcessQueue) == THREAD_SUSPENDED)
                                    sLog->outDebug("[Recv Thread] Resuming Queue thread: %d Status: %s", ProcessQueue, runStatus(threadMgr->ResumeThread(ProcessQueue)));
                            }
                        }
                    else
                        switch (sock->second->errnum())
                        {
                            case EBADF:
                                sLog->outString("[Recv Thread] sock: %d Error: EBADF", sock->second->GetFD());
                                FD_CLR(sock->second->GetFD(), &Recv);
                                socketMgr->CloseSocketByFd(sock->second->GetFD());
                                break;
                            case EAGAIN:
                                break;
                            default:
                                sLog->outError("[Recv Thread] Recv Error %d  (From %s  fd: %d)", sock->second->errnum(), sock->second->GetAddress(), sock->first);
                        }
                    r--;
                }
        socketMgr->CloseAllPendingSockets();
    }

    if (proto.OnUnload) // Enable Library to Init its variables
        proto.OnUnload();

    libLoader->close();
    socketMgr->CloseAllSockets();
    close(ServerSocket);
    threadMgr->JoinThead(ProcessQueue, NULL);
    sLog->outControl("[Main Process]  Exiting Application");
    return 1;
}
