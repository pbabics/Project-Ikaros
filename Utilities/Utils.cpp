#include "Utils.h"


std::string& refTrim(std::string &s)
{
    size_t lastchar = s.find_last_not_of(' ')+1;
    size_t firstchar = s.find_first_not_of(' ');
    if (lastchar == string::npos && lastchar > s.length())
        lastchar = s.length();
    if (firstchar == string::npos)
        firstchar = 0;
    s.erase(lastchar, s.length() - lastchar);
    s.erase(0, firstchar);
    return s;
}

int setNonblocking(int fd)
{
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

uint64 getMsTimeDiff(timeval a, timeval b)
{
    return (b.tv_sec - a.tv_sec) * 1000 + (b.tv_usec - a.tv_usec) / 1000;
}

uint64 getMsTimeDiffToNow(timeval a)
{
    timeval b;
    gettimeofday(&b, 0);
    return  (b.tv_sec - a.tv_sec) * 1000 + (b.tv_usec - a.tv_usec) / 1000;
}

int nanosleep(uint64 time)
{
    timespec t;
    t.tv_sec = 0;
    t.tv_nsec = time;
    return nanosleep(&t, NULL);
}
