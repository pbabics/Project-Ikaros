#include <dlfcn.h>
#include <string.h>

#ifndef __Shared
#define __Shared

// COMPILE Using  -ldl -rdynamic
#ifndef NULL
    #define NULL 0
#endif

class SharedLibrary
{
	public: 
        SharedLibrary(): LibName(NULL), lastErr(NULL), lib_handle(NULL), isopen(false) { }
		SharedLibrary(const char* Lib): LibName(NULL), lastErr(NULL), lib_handle(NULL), isopen(false) { open(Lib); }
		~SharedLibrary() { close(); }

		bool open(const char* LibLink);
		inline bool is_open() const { return isopen; }
		void close();

		void* findFunc(const char* Func);

		inline char* getError() const { return lastErr; }	
	    inline const char* GetLibraryPath() const { return LibName; }

    protected: 
	   const char  *LibName ;
	   char  *lastErr ;
	   void  *lib_handle ;
	   bool   isopen ;

};
#endif
