#include "Includes.h"
#include "Defs.h"
#include "Utils.h"
#include "ThreadMgr.h"
#include "SimpleLog.h"
#include "Shared.h"
#include "SignalHandler.h"
#include "SocketMgr.h"
#include "file.h"
#include "Handler.h"
#include "ConfigMgr.h"
#include "Daemon.h"
#include "revision.h"

#ifndef __App
#define __App

using Files::AsciiFile;


typedef void (*onConnect)(in_addr, int);
typedef void (*onRecieve)(in_addr, int, char*);
typedef void (*onSend)(char*);
typedef void (*onDisconnect)(in_addr, int);
typedef void (*onLoad)();
typedef void (*onUnload)();
typedef void (*tFunc)(void*);

typedef std::map<string, string, StringComparsionObject> Options;

struct Protocol
{
    onConnect       OnConnect;
    onRecieve       OnRecieve;
    onSend          OnSend;
    onDisconnect    OnDisconnect;

    onConnect       OnConnectDelayed;
    onRecieve       OnRecieveDelayed;
    onSend          OnSendDelayed;
    onDisconnect    OnDisconnectDelayed;

    onLoad          OnLoad;
    onUnload        OnUnload;
};

enum IntOptions
{
    CONFIG_INT_BIND_PORT,
    CONFIG_INT_LOG_LEVEL,
    CONFIG_INT_FREEZE_DETECTOR_MAX_DIFF,
    CONFIG_INT_DELAYED_SEND_DELAY,
    CONFIG_INT_DELAYED_RECV_DELAY,
    CONFIG_INT_DELAYED_CONNECT_DELAY,
    CONFIG_INT_DELAYED_DISCONNECT_DELAY,
    CONFIG_INT_MAX
};

enum StringOptions
{
    CONFIG_STRING_PID_FILE,
    CONFIG_STRING_WORKING_DIRECTORY,
    CONFIG_STRING_BIND_IP,
    CONFIG_STRING_PROTOCOL_NAME,
    CONFIG_STRING_LIBRARY_PATH,
    CONFIG_STRING_RECV_FUNC,
    CONFIG_STRING_DELAYED_RECV_FUNC,
    CONFIG_STRING_SEND_FUNC,
    CONFIG_STRING_DELAYED_SEND_FUNC,
    CONFIG_STRING_CONNECT_FUNC,
    CONFIG_STRING_DELAYED_CONNECT_FUNC,
    CONFIG_STRING_DISCONNECT_FUNC,
    CONFIG_STRING_DELAYED_DISCONNECT_FUNC,
    CONFIG_STRING_LOAD_FUNC,
    CONFIG_STRING_UNLOAD_FUNC,
    CONFIG_STRING_MAX
};

enum BoolOptions
{
    CONFIG_BOOL_DAEMONIZE,
    CONFIG_BOOL_LOG,
    CONFIG_BOOL_ENABLE_FREEZE_DETECTOR,
    CONFIG_BOOL_MAX
};

class PacketHandler;
class SignalHandler;
class ThreadMgr;
class ConnectionMgr;

extern SimpleLog* sLog;

void* CallFreezeDetector(void* obj);

class Runnable
{
    friend void* CallRunnable(void* obj);

    public:
        Runnable(): _thread(NULL) { }

        virtual void Run() { }

        void Kill() { if (_thread) _thread->Kill(); }
        void Suspend() { if (_thread) _thread->Suspend(); }
        void Terminate() { if (_thread) _thread->Terminate(); }
        void Continue() { if (_thread) _thread->Continue(); }

        void Execute();

        int GetStatus() const { if (_thread) return _thread->status; return 0; }
        Thread* GetThread() { return _thread; }

    protected:
        Thread* _thread;
};

class FreezeDetector : public Runnable
{
    public:
        FreezeDetector(uint64 maxDiffTime, uint64& diff):
        _maxDiffTime(maxDiffTime), _diff(diff), _exit(false), _pause(false), _active(false) { }

        void Run();
        void Exit() { _exit = true; }
        void Pause()  { _pause = true; }

    private:
        uint64 _maxDiffTime;
        uint64& _diff;
        bool _exit;
        bool _pause;
        bool _active;
};


class Application : public Daemon
{
    public:
            Application(int, char**, const char* conf);
            ~Application() { FileOptions.clear(); RunOptions.clear(); sLog->outControl("Terminating Application"); _uninitGlobals(); }
            bool inDebug() const { return debug; }
            bool inControl() const { return control; }

            uint32 Update();

            SignalHandler *sigHandler;
            SharedLibrary *libLoader;
            ConnectionMgr *socketMgr;
            ThreadMgr *threadMgr;
            PacketHandler *handler;
            FreezeDetector *freezeDetector;

            bool LoadLibrary();

            Protocol proto;

            inline bool Exiting() const { return terminate; }
            void Terminate();

            bool BoolConfigs[CONFIG_BOOL_MAX];
            int IntConfigs[CONFIG_INT_MAX];
            string StringConfigs[CONFIG_STRING_MAX];

    protected:
            Options FileOptions;
            Options RunOptions;
            bool debug;
            bool control;
            bool daemonize;
            bool libLoaded;
            bool terminate;
            int ServerSocket;

            string ApplicationAddress;

            uint64 _diffTime;
            timeval _lastUpdate;

    private:
            void ParseParams();
            void _initGlobals();
            void _uninitGlobals();
            void _InitServerSocket();
            void LoadConfigs();

            void LoadIntConfig(string ConfigName, uint32 Config, uint32 Default = 0);
            void LoadStringConfig(string ConfigName, uint32 Config, string Default = "");
            void LoadBoolConfig(string ConfigName, uint32 Config, bool Default = false);

            void LoadIntRunConfig(string ConfigName, string ShortFormat, uint32 Config);
            void LoadStringRunConfig(string ConfigName, string ShortFormat, uint32 Config);
            void LoadBoolRunConfig(string ConfigName, string ShortFormat, uint32 Config);

            void outDebugParams() const;
            void outDebugLibrary() const;
};
#endif
