#include <pthread.h>
#include <list>
#include <map>
#include <cstring>
#include <string>
#include <cstdio>
#include <signal.h>

#include "Defs.h"

#ifndef __Threads
#define __Threads

using std::string;
using std::map;
using std::list;

struct compStr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
  bool operator()(string s1, string s2) const
  {
    return s1.compare(s2) < 0;
  }
};

typedef std::map<string, pthread_mutex_t&, compStr> MutexMap ;
typedef std::map<string, pthread_cond_t&, compStr> ConditionMap ;
typedef std::map<string, uint32, compStr> ThreadNameMap;
typedef void* (*CalledFunction)(void*);

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
};

class ThreadMgr
{
    public:
        ThreadMgr(){ pThreads.clear(); }
        ~ThreadMgr(){ pThreads.clear(); }

        int CreateThread(string ThreadName, CalledFunction func, void* args);
        int CreateThread(CalledFunction func, void* args);

        bool CancelThread(uint32 ThreadId);
        bool CancelThread(string ThreadName);

        bool JoinThead(uint32 ThreadId, void** retval);
        bool JoinThead(string ThreadName, void** retval);

        int AddThread(string ThreadName, Thread* thread)
        {
            thread->id = pThreads.size();
            pThreads.push_back(*thread);
            pThreadNames.insert(std::pair<string, uint32>(ThreadName, thread->id));
            return thread->id;
        }

        static bool Exit(void* retval){ pthread_exit(retval); } //This Thread!!

        bool SendSignalToThread(uint32 ThreadId, int Signal);
        bool SendSignalToThread(string ThreadName, int Signal);

        bool KillThread(uint32 ThreadId) { return SendSignalToThread(ThreadId, SIGTERM); }
        bool KillThread(string ThreadName) { return SendSignalToThread(GetThreadIdByName(ThreadName), SIGTERM); }

        bool ResumeThread(uint32 ThreadId) { return SendSignalToThread(ThreadId, SIGCONT); }
        bool ResumeThread(string ThreadName) { return SendSignalToThread(GetThreadIdByName(ThreadName), SIGCONT); }

        uint32 GetThreadIdByName(string ThreadName);
        bool SetThreadName(uint32 ThreadId, string ThreadName);

        int GetThreadStatus(string ThreadName);
        int GetThreadStatus(uint32 ThreadId);
        int GetThreadStatus(int ThreadId)
        {
            if (ThreadId >= 0 && uint32(ThreadId) < pThreads.size())
                return GetThreadStatus(uint32(ThreadId));
            return 0;
        }
        int GetThreadStatus(pthread_t thread);

        void SetThreadStatus(string ThreadName, int Status);
        void SetThreadStatus(uint32 ThreadId, int Status);
        void SetThreadStatus(pthread_t thread, int Status);

        int CreateMutex(std::string MutexName, const pthread_mutexattr_t * Attr = NULL);
        static pthread_mutex_t _CreateMutex(const pthread_mutexattr_t * Attr = NULL)
        {
            pthread_mutex_t mutex;
            pthread_mutex_init(&mutex, Attr);
            return mutex;
        }
        void AddMutex(std::string MutexName, pthread_mutex_t mutex);
        int LockMutex(std::string MutexName);
        int UnlockMutex(std::string MutexName);
        int TryLockMutex(std::string MutexName);
        pthread_mutex_t& GetMutex(std::string MutexName);
        int DestroyMutex(std::string MutexName);
        bool MutexExists(std::string MutexName);

        int CreateCondition(std::string ConditionName, const pthread_condattr_t * Attr = NULL);
        int ConditionWait(std::string ConditionName, std::string MutexName);
        int ConditionSignal(std::string ConditionName);
        int ConditionBroadcast(std::string ConditionName);
        pthread_cond_t& GetCondition(std::string ConditionName);
        int DestroyCondition(std::string ConditionName);
        bool ConditionExists(std::string ConditionName);

        std::list<Thread>::iterator pgetThread(uint32 index);
        Thread& getThread(uint32 index);

        std::list<Thread>::iterator pgetThread(pthread_t thread);
        Thread& getThread(pthread_t thread);
        int getThreadId(pthread_t thread);

        std::list<Thread> pThreads ;
        MutexMap pMutexes;
        ConditionMap  pConditions;
        ThreadNameMap pThreadNames;
};


inline pthread_t GetThisThread()
{
    return pthread_self();
}
#endif
