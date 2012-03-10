#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <cassert>
#include <iostream>

#include <pthread.h> // for Mutex (compile with -pthread)

#ifndef __SimpleLog
#define __SimpleLog

using std::endl ;
using std::cout ;

extern pthread_mutex_t* writeMutex;

class SimpleLog
{
    public:
         SimpleLog(bool &C, bool &D, const char* logfile = NULL);
         ~SimpleLog();
         int outString(const char* format = "", ...);
         int outControl(const char* format = "", ...);
         int outDebug(const char* format = "", ...);
         int outError(const char* format = "", ...);

    private:
        const char* logf ;
        char* mdate;
        time_t d ;
        bool &Debug;
        bool &Control;
};

#endif
