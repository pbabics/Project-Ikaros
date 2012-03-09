#include "Daemon.h"


void Daemon::Daemonize(const char* lockFile, const char* workingDir)
{
    int i, lfp;
    char str[10];
    #pragma GCC diagnostic ignored "-Wunused-result"
    if (GetParentProcessId() == 1)
        return;
    i = fork();
    if (i < 0)
        exit(-1);
    if (i > 0)
        exit(0);
    SetCurrentProcessSessionId();
    for (i = getdtablesize(); i >= 0; --i)
        close(i);
    i = open("/dev/null", O_RDWR);
    dup(i);
    dup(i);
    umask(027);
    if (workingDir)
        SetWorkingDirectory(workingDir);

    if (lockFile)
    {
        lfp = open(lockFile, O_RDWR | O_CREAT, 0640);
        if (lfp < 0)
            exit(-1);
        if (lockf(lfp, F_TLOCK, 0) < 0)
            exit(0);

        sprintf(str, "%d\n", getpid());

        write(lfp, str, strlen(str));
    }
    signal(SIGCHLD,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    _isDaemon = true;
    #pragma GCC diagnostic warning "-Wunused-result"
}
