#include "ThreadMgr.h"


Thread* Thread::CreateThread(void * (*func)(void *), void *args)
{
    Thread* n = new Thread();
    if (pthread_create(&n->thread, NULL, func, args) == 0)
    {
        n->status = THREAD_ACTIVE;
        n->function = func;
        n->args = args;
        return n ;
    }
    return NULL ;
}



int ThreadMgr::CreateThread(void * (*func)(void *), void *args)
{
    Thread n;
    if (pthread_create(&n.thread, NULL, func, args) == 0)
    {
        n.id = pThreads.size();
        n.status = THREAD_ACTIVE;
        pThreads.push_back(n);
        return pThreads.size()-1 ;
    }
    return -1 ;
}

int ThreadMgr::CreateThread(string ThreadName, void * (*func)(void *), void *args)
{
    if (pThreadNames.find(ThreadName) != pThreadNames.end())
        return -1;

    int ret = CreateThread(func,args);
    if (ret != -1)
    {
        pThreadNames.insert(std::pair<string, uint32>(ThreadName, ret));
        return ret;
    }
    return -1 ;
}


bool ThreadMgr::CancelThread(uint32 threadId)
{
    if (threadId < pThreads.size())
        if (pthread_cancel(getThread(threadId)) == 0)
            return true;
    return false;
}

bool ThreadMgr::CancelThread(string ThreadName)
{
    return CancelThread(GetThreadIdByName(ThreadName));
}

bool ThreadMgr::JoinThead(uint32 threadId,void** retval)
{
    if (threadId < pThreads.size())
        if (pthread_join(getThread(threadId), retval) == 0)
            return true;
    return false;
}

bool ThreadMgr::JoinThead(string ThreadName,void** retval)
{
    return JoinThead(GetThreadIdByName(ThreadName), retval);
}

list<Thread>::iterator ThreadMgr::pgetThread(uint32 index)
{
    if (index > pThreads.size())
        return list<Thread>::iterator() ;
    list<Thread>::iterator  itr = pThreads.begin() ;
    advance(itr, index);
    return itr ;
}

Thread& ThreadMgr::getThread(uint32 index)
{
    return *pgetThread(index) ;
}

list<Thread>::iterator ThreadMgr::pgetThread(pthread_t pthread)
{
    list<Thread>::iterator  itr = pThreads.begin() ;
    for (; itr != pThreads.end(); itr++)
        if (pthread_equal(itr->thread, pthread))
            return itr ;
    return pThreads.end();
}

Thread& ThreadMgr::getThread(pthread_t thread)
{
    list<Thread>::iterator it = pgetThread(thread);
    if (it != pThreads.end())
        return *it ;
    return *(Thread*)(NULL);
}

int ThreadMgr::getThreadId(pthread_t thread)
{
    if (&getThread(thread))
        return getThread(thread).id ;
    return -1;
}

bool ThreadMgr::SendSignalToThread(uint32 ThreadId, int Signal)
{
    if (ThreadId >= pThreads.size())
        return false;
    return pthread_kill(getThread(ThreadId), Signal) == 0;
}

bool ThreadMgr::SendSignalToThread(string ThreadName, int Signal)
{
    return SendSignalToThread(GetThreadIdByName(ThreadName), Signal);
}

uint32 ThreadMgr::GetThreadIdByName(string ThreadName)
{
    if (pThreadNames.find(ThreadName) == pThreadNames.end())
        return pThreadNames.size();
    return pThreadNames.find(ThreadName)->second;
}

bool ThreadMgr::SetThreadName(uint32 ThreadId, string ThreadName)
{
    if (pThreadNames.find(ThreadName) != pThreadNames.end())
        return false;
    pThreadNames.insert(std::pair<string, uint32>(ThreadName, ThreadId));
    return true;
}

int ThreadMgr::GetThreadStatus(string ThreadName)
{
    return GetThreadStatus(GetThreadIdByName(ThreadName));
}

int ThreadMgr::GetThreadStatus(pthread_t thread)
{
    if (getThreadId(thread) != -1)
        return GetThreadStatus(uint32(getThreadId(thread)));
    return 0;
}

int ThreadMgr::GetThreadStatus(uint32 ThreadId)
{
    if (ThreadId >= pThreads.size())
        return -1;
    return getThread(ThreadId).status;
}

void ThreadMgr::SetThreadStatus(string ThreadName, int Status)
{
    SetThreadStatus(GetThreadIdByName(ThreadName), Status);
}

void ThreadMgr::SetThreadStatus(pthread_t thread, int Status)
{
    if (getThreadId(thread) != -1)
        SetThreadStatus(uint32(getThreadId(thread)), Status);
}

void ThreadMgr::SetThreadStatus(uint32 ThreadId, int Status)
{
    if (ThreadId >= pThreads.size())
        return;
    if (Status > THREAD_STATUS_BEGIN && Status < THREAD_STATUS_END)
        getThread(ThreadId).status = Status;
}

/*                              */
/*          Mutexes             */
/*                              */
int ThreadMgr::CreateMutex(std::string MutexName, const pthread_mutexattr_t * Attr)
{
    if (pMutexes.find(MutexName) != pMutexes.end())
        return false;
    int ret = 0;
    pthread_mutex_t mutex;
    ret = pthread_mutex_init(&mutex, Attr);
    pMutexes.insert(std::pair<string, pthread_mutex_t&>(MutexName, mutex));
    return ret;
}

void ThreadMgr::AddMutex(std::string MutexName, pthread_mutex_t mutex)
{
    if (pMutexes.find(MutexName) != pMutexes.end())
        return;

    pMutexes.insert(std::pair<string, pthread_mutex_t&>(MutexName, mutex));
}

int ThreadMgr::LockMutex(std::string MutexName)
{
    if (pMutexes.find(MutexName) == pMutexes.end())
        return false;

    int ret = 0;
    ret = pthread_mutex_lock(&GetMutex(MutexName));
    return ret;
}

int ThreadMgr::UnlockMutex(std::string MutexName)
{
    if (pMutexes.find(MutexName) == pMutexes.end())
        return false;

    int ret = 0;
    ret = pthread_mutex_unlock(&GetMutex(MutexName));
    return ret;
}

int ThreadMgr::TryLockMutex(std::string MutexName)
{
    if (pMutexes.find(MutexName) == pMutexes.end())
        return false;

    int ret = 0;
    ret = pthread_mutex_trylock(&GetMutex(MutexName));
    return ret;
}

int ThreadMgr::DestroyMutex(std::string MutexName)
{
    if (pMutexes.find(MutexName) == pMutexes.end())
        return false;

    int ret = 0;
    MutexMap::iterator it = pMutexes.find(MutexName);
    ret = pthread_mutex_destroy(&(it->second));
    pMutexes.erase(it);
    return ret;
}

pthread_mutex_t& ThreadMgr::GetMutex(std::string MutexName)
{
    if (pMutexes.find(MutexName) == pMutexes.end())
        return *(pthread_mutex_t*)NULL;

    MutexMap::iterator it = pMutexes.find(MutexName);
    return it->second;
}

bool ThreadMgr::MutexExists(std::string MutexName)
{
    if (pMutexes.find(MutexName) != pMutexes.end())
        return true;
    return false;
}

/*                              */
/*          Conditions          */
/*                              */

int ThreadMgr::CreateCondition(std::string ConditionName, const pthread_condattr_t * Attr)
{
    if (pConditions.find(ConditionName) != pConditions.end())
        return false;
    int ret = 0;
    pthread_cond_t condition;
    ret = pthread_cond_init(&condition,Attr);
    pConditions.insert(std::pair<string, pthread_cond_t&>(ConditionName,condition));
    return ret;
}

int ThreadMgr::ConditionWait(std::string ConditionName, std::string MutexName)
{
    if (pConditions.find(ConditionName) == pConditions.end() || pMutexes.find(MutexName) == pMutexes.end())
        return false;

    return pthread_cond_wait(&GetCondition(ConditionName),&GetMutex(MutexName));
}

int ThreadMgr::ConditionSignal(std::string ConditionName)
{
    if (pConditions.find(ConditionName) == pConditions.end())
        return false;

    return pthread_cond_signal(&GetCondition(ConditionName));
}

int ThreadMgr::ConditionBroadcast(std::string ConditionName)
{
    if (pConditions.find(ConditionName) == pConditions.end())
        return false;

    return pthread_cond_broadcast(&GetCondition(ConditionName));
}

pthread_cond_t& ThreadMgr::GetCondition(std::string ConditionName)
{
    if (pConditions.find(ConditionName) == pConditions.end())
        return *(pthread_cond_t*)NULL;

    ConditionMap::iterator it = pConditions.find(ConditionName);
    return it->second;
}

int ThreadMgr::DestroyCondition(std::string ConditionName)
{
    if (pConditions.find(ConditionName) == pConditions.end())
        return false;
    int ret = 0;
    ConditionMap::iterator it = pConditions.find(ConditionName);
    ret = pthread_cond_destroy(&it->second);
    pConditions.erase(it);
    return ret;
}

bool ThreadMgr::ConditionExists(std::string ConditionName)
{
    if (pConditions.find(ConditionName) != pConditions.end())
        return true;
    return false;
}
