#include "Includes.cpp"
#include "Adds.h"
#include "defines.h"

#include "ConfigMgr.cpp"

#ifdef __cplusplus
extern "C" 
{
#endif

void processRecieve(in_addr /* address */, int fd, char* data);
void Init();
void InitStrings();
void InitErrorPages();
void LoadConfig();

class HTTPResponse
{
    public:
        static void SendErrorPage(Socket* socket, uint32 ErrorId);
        static void SendDirectPage(Socket* socket, string file);
        static void SendHeadPage(Socket* socket, string file);
        static void KillSpartans(Socket* socket);
};

#ifdef __cplusplus
}
#endif

struct SeparateThreadedSendingData
{
    SeparateThreadedSendingData(Socket* s, const char* d, size_t l): sock(s), data(d), length(l), dat(NULL), stream(NULL) { }
    SeparateThreadedSendingData(Socket* s, BinnaryData* d): sock(s),  data(NULL), length(0), dat(d), stream(NULL) { }
    SeparateThreadedSendingData(Socket* s, stringstream* d): sock(s),  data(NULL), length(0), dat(NULL), stream(d) { }
    Socket* sock;
    const char* data;
    size_t length;
    BinnaryData* dat;
    stringstream* stream;
};

std::map<int, const char*> errorStrings;
std::map<int, const char*> errorPages;
ConfigMgr* configMgr;
SimpleLog* protoLog;

enum BoolConfigs
{
    CONFIG_BOOL_ALLOW_DEFAULT_INDEX,
    CONFIG_BOOL_DEBUG,
    CONFIG_BOOL_CONTROL,
    MAX_BOOL_CONFIGS
};

enum StringConfigs
{
    CONFIG_STRING_DEFAULT_INDEX_FILE,
    CONFIG_STRING_HOSTNAME,
    CONFIG_STRING_ROOT_DIR,
    MAX_STRING_CONFIGS
};

enum IntConfigs
{
    CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE,
    MAX_INT_CONFIGS
};

bool boolConfigs[MAX_BOOL_CONFIGS];
string stringConfigs[MAX_STRING_CONFIGS];
int intConfigs[MAX_STRING_CONFIGS];

