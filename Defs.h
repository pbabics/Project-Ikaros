typedef unsigned long uint64;
typedef unsigned int uint32 ;
typedef unsigned short uint16 ;
typedef unsigned char uint8 ;
typedef unsigned char byte ;
typedef unsigned char Byte ;

typedef long int64;
typedef int int32 ;
typedef short int16 ;
typedef char int8 ;

typedef void (*handleFunc)(char*, int);// Used for opcode Handlers

#define IN_KILOBYTES 1024
#define IN_MEGABYTES IN_KILOBYTES * IN_KILOBYTES

#define IN_MICROSECONDS 1000
#define IN_MILLISECONDS IN_MICROSECONDS * IN_MICROSECONDS
#define IN_SECONDS IN_MILLISECONDS * IN_MILLISECONDS
