#include "SimpleLog.h"


SimpleLog::SimpleLog(bool &C, bool &D, const char* logfile):
logf(NULL), mdate(NULL), Debug(D), Control(C)
{
    if (logfile)
    {
        logf = logfile;
        fclose(fopen(logf, "w"));
    }
    mdate= new char[256];
    if (!writeMutex)
        pthread_mutex_init(writeMutex, NULL);
}

SimpleLog::~SimpleLog()
{
    if (mdate)
        delete mdate;
    if (writeMutex)
        pthread_mutex_destroy(writeMutex);
}

int SimpleLog::outControl(const char* format, ...)
{
    if (Control)
    {
        pthread_mutex_lock(writeMutex);
        va_list arg;
        int done = 0;
        if (strlen(format))
        {
            memset(mdate, 0, 256);
            time(&d);
            strftime(mdate, 256, "%Y-%m-%d %H:%M:%S", localtime(&d));
            printf("%s \t", mdate);
            va_start(arg, format);
            done = vfprintf(stdout, format, arg);
            va_end(arg);
        }
        if (logf)
        {
            FILE* log = fopen(logf, "a");
            assert(log);
            if (strlen(format))
            {
                va_start(arg, format);
                fprintf(log,"%s \t", mdate);
                vfprintf(log, format, arg);
                va_end(arg);
            }
            fprintf(log, "\n");
            fclose(log);
        }
        std::cout << std::endl;
        std::cout.flush();
        pthread_mutex_unlock(writeMutex);
        return done;
    }
    else
        return 0;
}

int SimpleLog::outString(const char* format, ...)
{
    va_list arg;
    int done = 0;
    pthread_mutex_lock(writeMutex);
    if (strlen(format))
    {
        memset(mdate, 0, 256);
        time(&d);
        strftime(mdate, 256, "%Y-%m-%d %H:%M:%S", localtime(&d));
        printf("%s \t", mdate);
        va_start(arg, format);
        done = vfprintf(stdout, format, arg);
        va_end(arg);
    }

    if (logf)
    {
        FILE* log = fopen(logf, "a");
        assert(log);
        if (strlen(format))
        {
            va_start(arg, format);
            fprintf(log, "%s \t", mdate);
            vfprintf(log, format, arg);
            va_end(arg);
        }
        fprintf(log, "\n");
        fclose(log);
    }
    std::cout << std::endl;
    std::cout.flush();
    pthread_mutex_unlock(writeMutex);
    return done;
}

int SimpleLog::outDebug(const char* format, ...)
{
    if (Debug)
    {
        pthread_mutex_lock(writeMutex);
        va_list arg;
        int done = 0;
        if (strlen(format))
        {
            memset(mdate, 0, 256);
            time(&d);
            strftime(mdate, 256, "%Y-%m-%d %H:%M:%S", localtime(&d));
            printf("%s \t", mdate);
            va_start(arg, format);
            done = vfprintf(stdout, format, arg);
            va_end(arg);
        }
        if (logf)
        {
            FILE* log = fopen(logf, "a");
            assert(log);
            if (strlen(format))
            {
                va_start(arg, format);
                fprintf(log, "%s \t", mdate);
                vfprintf(log, format, arg);
                va_end(arg);
            }
            fprintf(log, "\n");
            fclose(log);
        }
        va_end(arg);
        std::cout << std::endl;
        std::cout.flush();
        pthread_mutex_unlock(writeMutex);
        return done;
    }
    else
        return 0;
}

int SimpleLog::outError(const char* format, ...)
{
    va_list arg;
    int done = 0;
    pthread_mutex_lock(writeMutex);
    if (strlen(format))
    {
        memset(mdate, 0, 200);
        time(&d);
        strftime(mdate, 256, "%Y-%m-%d %H:%M:%S", localtime(&d));
        printf("%s \t[ERROR] ", mdate);
        va_start(arg, format);
        done = vfprintf(stdout, format, arg);
        va_end(arg);
    }
    if (logf)
    {
        FILE* log = fopen(logf, "a");
        assert(log);
        if (strlen(format))
        {
            va_start(arg, format);
            fprintf(log, "%s \t", mdate);
            fprintf(log, "[ERROR] ");
            vfprintf(log, format, arg);
            va_end(arg);
        }
        fprintf(log, "\n");
        fclose(log);
    }
    std::cout << std::endl;
    std::cout.flush();
    pthread_mutex_unlock(writeMutex);
    return done;
}
