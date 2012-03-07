#include "Includes.cpp"
#include "SMTP.cpp"
#include "Adds.cpp"

#include "defines.h"


#ifdef __cplusplus
extern "C" 
{
#endif

void processRecieve(in_addr /* address */, int fd, char* data);
void processConnect(in_addr /* address */, int fd);
void processDisconnect(in_addr /* address */, int fd);
void Init();



#ifdef __cplusplus
}
#endif

enum SMTPStates
{
    SMTP_GREETINGS          = 0,
    SMTP_HELLO              = 1,
    SMTP_MAIL_FROM          = 2,
    SMTP_MAIL_RCPT          = 3,
    SMTP_MAIL_DATA_START    = 4,
    SMTP_MAIL_DATA          = 5,
    SMTP_QUIT               = 6,
    SMTP_EHLO               = 7,
};

struct SMTPSessionData
{
    string Host;
    string mailFrom;
    std::vector<string> mailTo;
    string data;
};

std::map<int, int> sessionStatus;
std::map<int, SMTPSessionData> sessionData;

