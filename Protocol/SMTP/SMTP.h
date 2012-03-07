#include "Includes.cpp"
#include "defines.h"

#include "ConfigMgr.cpp"

std::map<int, const char*> responseCodeMessage;

#define Greetings "Welcome to Ikaros SMTP Server"

bool Debug= true, Control= true;
SimpleLog* protoLog;
ConfigMgr* configMgr;

#ifdef __cplusplus
extern "C" 
{
#endif

class SMTP
{
    public:
        static void SendGreetings(Socket* sock);
        static void SendHelloResponse(Socket* sock, std::string);
        static void SendCommandResponse(Socket* sock, int responseCode);

};

#ifdef __cplusplus
}
#endif

enum StringConfigs
{
    CONFIG_STRING_GREETINGS,
    MAX_STRING_CONFIGS
};

string stringConfigs[MAX_STRING_CONFIGS];
