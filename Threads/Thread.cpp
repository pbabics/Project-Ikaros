#include "Thread.h"

Thread* Thread::CreateThread(CalledFunction func, void *args)
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

int Thread::Kill() const
{
    return pthread_kill(thread, SIGKILL);
}

int Thread::Terminate() const
{
    return pthread_kill(thread, SIGTERM);
}

int Thread::Interrupt() const
{
    return pthread_kill(thread, SIGINT);
}

int Thread::Continue() const
{
    return pthread_kill(thread, SIGCONT);
}

int Thread::SendSignal(int sig)
{
    return pthread_kill(thread, sig);
}

void Thread::Suspend()
{
    sigset_t suspendSig;
    int sig = SIGCONT;
    sigemptyset(&suspendSig);
    sigaddset(&suspendSig, SIGCONT);
    sigaddset(&suspendSig, SIGINT);
    sigaddset(&suspendSig, SIGTERM);
    status = THREAD_SUSPENDED;
    sigwait(&suspendSig, &sig);
    status = THREAD_ACTIVE;
}

void Thread::SuspendThisThread()
{
    sigset_t suspendSig;
    int sig = SIGCONT;
    sigemptyset(&suspendSig);
    sigaddset(&suspendSig, SIGCONT);
    sigaddset(&suspendSig, SIGINT);
    sigaddset(&suspendSig, SIGTERM);
    sigwait(&suspendSig, &sig);
}
