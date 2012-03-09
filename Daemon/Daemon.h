#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef __Daemon
#define __Daemon

#define MAX_BUFFER_SIZE 1024

class Process
{
    public:
        pid_t GetProcessId() const { return getpid(); }

        pid_t GetCurrentProcessSessionId() const { return getsid(getpid()); }
        pid_t SetCurrentProcessSessionId() const { return setsid(); }

        pid_t GetProcessSessionId(pid_t process) const { return getsid(process); }

        pid_t GetParentProcessId() const { return getppid(); }

        pid_t GetCurrentProcessGroupId() const { return getpgrp(); }
        int SetCurrentProcessGroupId(pid_t newGroupId) { return setpgid(0, newGroupId); }
        int SetCurrentProcessGroupId() { return setpgrp(); }

        pid_t GetProcessGroupId(pid_t process) const { return getpgid(process); }
        int SetProcessGroupId(pid_t process, pid_t newGroupId) { return setpgid(process, newGroupId); }

        char* GetWorkingDirectory() const
        {
            char wdir[MAX_BUFFER_SIZE];
            memset(wdir, 0, MAX_BUFFER_SIZE);
            return getcwd(wdir, MAX_BUFFER_SIZE);
        }
        int SetWorkingDirectory(const char* directory)
        {
            return chdir(directory);
        }
};


class Daemon : public Process
{
    public:
        Daemon(): _isDaemon(false) { }
        void Daemonize(const char* lockfile = NULL, const char* workingDir = NULL);
        bool isDaemon() const { return _isDaemon; }

    private:
        bool _isDaemon;
};

#endif // __Daemon
