#include "Shared.h"

bool SharedLibrary::open(const char* LibLink)
{
    if (isopen)
        close();
    lib_handle  = dlopen(LibLink, RTLD_LAZY);
    if (!lib_handle)
    {
        char* buf = dlerror();
        if (lastErr)
            delete lastErr;
        lastErr = new char[strlen(buf)];
        strcpy(lastErr, buf);
        return isopen = false ;
    }
    LibName = LibLink ;
    return isopen = true ;
}

void * SharedLibrary::findFunc(const char* Func)
{
    if (!isopen)
    {
        lastErr = const_cast<char*>("Library is not opened");
        return NULL ;
    }
    void * func = dlsym(lib_handle,Func);
    if (!func)
    {
        char* buf = dlerror();
        if (lastErr)
            delete lastErr;
        lastErr = new char[strlen(buf)];
        strcpy(lastErr, buf);
        return NULL;
    }
    return func ;
}

void SharedLibrary::close()
{
    if(isopen)
        dlclose(lib_handle);
}
