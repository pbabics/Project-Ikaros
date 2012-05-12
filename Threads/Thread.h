#include <pthread.h>
#include <signal.h>

#include "Defs.h"


#ifndef __Thread
#define __Thread

enum ThreadStatus
{
    THREAD_STATUS_BEGIN = 1,
    THREAD_ACTIVE,
    THREAD_SUSPENDED,
    THREAD_EXIT,
    THREAD_STATUS_END
};

struct Thread
{
    Thread(): id(0), status(0), attributes(0) { }
    pthread_t thread;
    uint32  id;
    int status;
    uint32 attributes;
    CalledFunction function;
    void* args;

    static Thread* CreateThread(void* (*func)(void*), void* args);
    operator pthread_t() { return thread; }
    operator int() { return id; }

    int Kill() const;
    int Terminate() const;
    int Interrupt() const;
    int Continue() const;
    int SendSignal(int sig);

    void Suspend();
    static void SuspendThisThread();
};

inline pthread_t GetThisThread()
{
    return pthread_self();
}

#endif // __Thread
