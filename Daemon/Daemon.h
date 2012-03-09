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

class Daemon
{
    public:
        Daemon(): _isDaemon(false) { }
        void Daemonize(const char* lockfile = NULL, const char* workingDir = NULL);
        bool isDaemon() const { return _isDaemon; }

    private:
        bool _isDaemon;
};

#endif // __Daemon
