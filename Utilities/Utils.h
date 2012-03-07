#include "Includes.h"

#ifndef __Utils
#define __Utils

std::string& refTrim(std::string &s);
int setNonblocking(int fd);


inline const char* toString(bool b)
{
    return b ? "true" : "false";
}

inline const char* runStatus(bool b)
{
    return b? "Running" : "Paused";
}


inline float AddPctF(float& f, const int pct)
{
    return f + f * pct / 100.f;
}

inline float CalculatePctF(float& f, const int pct)
{
    return f * pct / 100.f;
}

inline int AddPctN(int& i, const int pct)
{
    return i + i * pct / 100.f;
}

inline int CalculatePctN(int& i, const int pct)
{
    return i * pct / 100.f;
}

struct StringComparsionObject
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
  bool operator()(std::string s1, std::string s2) const
  {
    return s1.compare(s2) < 0;
  }
};

template <class T>
bool isWithinList(T valueToCompare, unsigned int count, ...)
{
    va_list v;
    va_start(v, count);

    for (unsigned int i = 0; i < count; i++)
    {
        T arg = va_arg(v, T);
        if (valueToCompare == arg)
        {
            va_end(v);
            return true;
        }
    }
    va_end(v);
    return false;
}

#endif // __Utils
