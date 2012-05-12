#include "Includes.cpp"
#include "Adds.cpp"
#include "FTP.cpp"
#include "FileAccessor.cpp"


#ifdef __cplusplus
extern "C"
{
#endif

void processRecieve(in_addr /* address */, int fd, char* data);
void processConnect(in_addr /* address */, int fd);
void processDisconnect(in_addr /* address */, int fd);
void Init();
void DeInit();


#ifdef __cplusplus
}
#endif

