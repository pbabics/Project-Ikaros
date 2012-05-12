#include "Includes.cpp"
#include "defines.h"
#include "Utils.h"
#include "Database.cpp"
#include "ConfigMgr.cpp"

#ifndef __FTP
#define __FTP



enum FTPStates
{
    FTP_LOGIN_USER = 0,
    FTP_LOGIN_PASS,
    FTP_CHANGE_WORKING_DIRECTORY,
    FTP_PRINT_WORKING_DIRECTORY,
    FTP_CHANGE_WORKING_DIRECTORY_TO_PARENT,
    FTP_LIST,
    FTP_QUIT,
    FTP_STORAGE_STORE,
    FTP_TRANSFER_TYPE,
    FTP_STORAGE_RETRIEVE,
    FTP_STORAGE_DELETE,
    FTP_MAKE_DIR,
    FTP_REMOVE_DIR,
    FTP_HELP,
    FTP_NOOP,
    FTP_SYST,
    FTP_SESSION_PORT,
    FTP_SESSION_PASSIVE,
    FTP_UNKNOWN,
};

enum TransferType
{
        TRANSFER_BINARY,
        TRANSFER_ASCII
};

class IP
{
    public:
        IP(): ipC(NULL) { memset(ipB, 0, 4); ByteToInAddr(ipB); }
        IP(byte i[4]) { ByteToInAddr(i); ipC = inet_ntoa(ipI); }
        IP(in_addr i) { ipC = inet_ntoa(i); ipI = i; InAddrToByte(i); }
        IP(char* i): ipC(i) { inet_aton(i, &ipI); InAddrToByte(ipI); }


        void InAddrToByte(in_addr i) { memcpy(ipB, &i.s_addr, 4); }
        void ByteToInAddr(byte* i) { memcpy(&ipI.s_addr, i, 4); }

        char* ToChar() const { return ipC; }

        void Restore() { ByteToInAddr(ipB); ipC = inet_ntoa(ipI); }

        operator in_addr() { return ipI; }
        operator char*() { return ipC; }
        byte& operator [](const int location) { return ipB[location]; }

    protected:
        byte ipB[4]; // addr
        in_addr ipI;
        char* ipC;
};

struct SessionSendStruct;
struct SessionRecvStruct;

struct SessionDataStruct
{
    SessionDataStruct(): username(""), password(""), parentDir("/"), actualDir("/"), homeDirectory("."),
      port(20), passiveSock(0), loginPrompted(false), loggedIn(false), isPassive(false), DTPActive(false), abortTranfser(false),
      activeSend(NULL), activeRecv(NULL) { }

    string username;
    string password;
    string parentDir;
    string actualDir;
    string homeDirectory;
    TransferType transferType;

    IP ip;
    uint16 port;

    int passiveSock;

    bool loginPrompted;
    bool loggedIn;
    bool isPassive;

    bool DTPActive;
    bool abortTranfser;

    SessionSendStruct* activeSend;
    SessionRecvStruct* activeRecv;
};

struct SessionSendStruct
{
        SessionSendStruct(SessionDataStruct& sdata, Socket* sock, char* data, size_t length, const char* file = NULL):
        s(sdata), so(sock), d(data), l(length)
        {
            if (file)
            {
                f = new char[strlen(file)+1];
                memset(f, 0, strlen(file)+1);
                memcpy(f, file, strlen(file));
            }
            else
                f = NULL;
        }

        SessionDataStruct& s;
        Socket* so;
        char* d;
        char* f;
        size_t  l;
};

struct SessionRecvStruct
{
        SessionRecvStruct(SessionDataStruct& sdata, Socket* sock, const char* fileName):
        s(sdata), so(sock), f(new char[strlen(fileName) + 1])
        {
            char* fi = new char[strlen(fileName) + 1];
            memset(fi, 0, strlen(fileName) + 1);
            memcpy(fi, fileName, strlen(fileName));
            f = fi;
        }
        ~SessionRecvStruct() { delete f; }

        SessionDataStruct& s;
        Socket* so;
        const char* f;
};

std::map<int, int> sessionStatus;
std::map<int, SessionDataStruct> sessionData;

SimpleLog* protoLog;
ConfigMgr* configMgr;
Database* db;

std::map<int, const char*> responseCodeMessage;

#ifdef __cplusplus
extern "C"
{
#endif

class FTP
{
    public:
        static void SendGreetings(Socket* sock);
        static void SendHelloResponse(Socket* sock, std::string);
        static void SendCommandResponse(Socket* sock, int responseCode);
        static void SendSpecialCommandResponse(Socket* sock, int responseCode, string text);
        static void SendOverDTP(int fd, char* data, size_t dataLength, const char* file = NULL);
        static void RecieveOverDTP(int fd, const char* fileName);
        static void InitPassiveSock(SessionDataStruct& session);
};

#ifdef __cplusplus
}
#endif


enum BoolConfigs
{
    CONFIG_BOOL_DEBUG,
    CONFIG_BOOL_CONTROL,
    CONFIG_BOOL_USE_DB_AUTH,
    CONFIG_BOOL_ALLOW_GUESTS,
    MAX_BOOL_CONFIGS
};

enum StringConfigs
{
    CONFIG_STRING_GREETINGS,
    CONFIG_STRING_ROOT_DIRECTORY,
    CONFIG_STRING_DATABASE_HOST,
    CONFIG_STRING_DATABASE_USERNAME,
    CONFIG_STRING_DATABASE_PASSWORD,
    CONFIG_STRING_DATABASE_NAME,
    MAX_CONFIG_STRING
};

enum IntConfigs
{
    CONFIG_INT_MAX_DATA_SEGMENT_SIZE,
    CONFIG_INT_SEND_WAIT_TIME,
    CONFIG_INT_SEND_ERROR_WAIT_TIME,
    CONFIG_INT_RECV_WAIT_TIME,
    CONFIG_INT_RECV_ERROR_WAIT_TIME,
    MAX_CONFIG_INT

};

bool boolConfigs[MAX_BOOL_CONFIGS];
string stringConfigs[MAX_CONFIG_STRING];
int intConfigs[MAX_CONFIG_INT];

#endif // __FTP
