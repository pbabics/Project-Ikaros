#ifndef __PacketHandler
#define __PacketHandler

#include "Includes.h"

#include "SimpleLog.h"
#include "file.h"
#include "Defs.h"
#include "Application.h"

using namespace Files;

enum Events
{
    EVENT_CONNECT = 0,
    EVENT_RECIEVE,
    EVENT_SEND,
    EVENT_DISCONNECT
};

struct Event
{
    Event(byte e, int f, in_addr addr, BinnaryData& dat = *(new BinnaryData()), int recv = 0, int err = 0):
    EventType(e), fd(f), address(addr), data(dat), recieved(recv), errorcode(err)
    {
    }
    byte EventType;
    int fd;
    in_addr address;
    BinnaryData& data;
    int recieved;
    int errorcode;
};

struct DelayedEvent : Event
{
    DelayedEvent(byte e, int f, in_addr addr, uint64 delay, BinnaryData& dat = *(new BinnaryData()), int recv = 0, int err = 0):
    Event(e, f, addr, dat, recv, err),
    msDelay(delay)
    {
        updated.tv_sec = 0;
        updated.tv_usec = 0;
        gettimeofday(&updated, 0);
    }

    DelayedEvent(byte e, int f, in_addr addr, BinnaryData& dat = *(new BinnaryData()), int recv = 0, int err = 0):
    Event(e, f, addr, dat, recv, err),
    msDelay(1000)
    {
        updated.tv_sec = 0;
        updated.tv_usec = 0;
        gettimeofday(&updated, 0);
    }

    uint64 msDelay;
    timeval updated;

    bool Execute() const { return !msDelay; }
    void Update(uint64 diff) { msDelay -= msDelay > diff? diff : msDelay; gettimeofday(&updated, 0); }
    void SetDelay(uint64 delay) { msDelay = delay; gettimeofday(&updated, 0); }
};

class Application;
extern SimpleLog* sLog;
extern Application* app;

class PacketHandler
{
    public:
            typedef std::queue<Event> EventQueue;
            typedef std::list<DelayedEvent> DelayedEventQueue;
            PacketHandler();
            ~PacketHandler();

            void AddEvent(Event event);
            void AddDelayedEvent(DelayedEvent event);
            void ProcessQueue();
            void UpdateDelayed();
            void Terminate();

    private:
            SignalHandler* sigHandler;

            bool process;

            EventQueue queue;
            DelayedEventQueue delayedQueue;
};

#endif
