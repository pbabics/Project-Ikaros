#include "Base.h"


void InitMessages();
void LoadConfigs();

void Init()
{
    configMgr = new ConfigMgr("smtp.conf", NULL, 0);
    protoLog = new SimpleLog(Control, Debug, "Protocol.log");
    InitMessages();
    LoadConfigs();
}

void InitMessages()
{
    // Connect
    responseCodeMessage.insert( make_pair(220, "Welcome to Ikaros SMTP Server"));
    responseCodeMessage.insert( make_pair(421, "Service not avaliable, closing transmission channel"));
    // EHLO
    responseCodeMessage.insert( make_pair(550, "Not Implemented"));

    responseCodeMessage.insert( make_pair(250, "Ok"));
    responseCodeMessage.insert( make_pair(354, "Start mail input; end with <CRLF>.<CRLF>"));
}

void LoadConfigs()
{
    stringConfigs[CONFIG_STRING_GREETINGS] = configMgr->LoadStringConfig("Greetings", "Welcome to Ikaros SMTP Server");
    protoLog->outDebug("CONFIG_STRING_GREETINGS= '%s'", stringConfigs[CONFIG_STRING_GREETINGS].c_str());
    protoLog->outDebug();
}

void processConnect(in_addr /* address */, int fd)
{
    Socket* sock = app->socketMgr->GetSocketByFd(fd);
    SMTP::SendGreetings(sock);
    if (sessionStatus.find(fd) == sessionStatus.end())
        sessionStatus.insert(make_pair(fd, SMTP_GREETINGS));
    sessionStatus[fd] = SMTP_HELLO;
}


void processDisconnect(in_addr /* address */, int fd)
{
    if (sessionStatus.find(fd) != sessionStatus.end())
        sessionStatus.erase(fd);
    if (sessionData.find(fd) != sessionData.end())
        sessionData.erase(fd);
    protoLog->outControl("Connection form %d  closed", fd);
}

void processRecieve(in_addr /* address */, int fd, char* data)
{
    Socket* sock = app->socketMgr->GetSocketByFd(fd);
    BinnaryData dat(data, strlen(data));

    string strdata = dat.TextLike();
    if (sessionStatus[fd] != SMTP_MAIL_DATA)
    {
        strdata = trim(data);
        string command; // Command is four character string
        dat >> command;
        command = trim(command);
        if (lower(command).compare("helo") == 0)
            sessionStatus[fd] = SMTP_HELLO;

        if (lower(command).compare("mail") == 0)
            sessionStatus[fd] = SMTP_MAIL_FROM;

        if (lower(command).compare("rcpt") == 0)
            sessionStatus[fd] = SMTP_MAIL_RCPT;

        if (lower(command).compare("data") == 0)
            sessionStatus[fd] = SMTP_MAIL_DATA_START;

        if (lower(command).compare("quit") == 0)
            sessionStatus[fd] = SMTP_QUIT;

        protoLog->outControl("Recieved Command:  '%s'  Data Length:  %u Data: '%s'", trim(command).c_str(), dat.size(), strdata.c_str());
    }


    switch (sessionStatus[fd])
    {
        case SMTP_GREETINGS: // Newly Connected handled in processConnect
        break;
        case SMTP_HELLO:
        {
            string name;
            dat >> name;
            protoLog->outDebug("Recieved Hello name: %s", name.c_str());
            sessionData[fd].Host = name;
            SMTP::SendHelloResponse(sock, name);
            break;
        }
        case SMTP_MAIL_FROM:
        {
            string commandData;
            dat >> commandData;
            commandData = trim(commandData);
            string mailAddr = commandData.substr(commandData.find_first_of('<')+1, commandData.find_last_of('>') - commandData.find_first_of('<')-1);
            protoLog->outDebug("Sending mail from: %s", mailAddr.c_str());
            sessionData[fd].mailFrom = mailAddr;
            SMTP::SendCommandResponse(sock, 250);
            break;
        }
        case SMTP_MAIL_RCPT:
        {
            string commandData;
            dat >> commandData;
            commandData = trim(commandData);
            string mailAddr = commandData.substr(commandData.find_first_of('<')+1, commandData.find_last_of('>') - commandData.find_first_of('<')-1);
            protoLog->outDebug("Sending mail To: %s", mailAddr.c_str());
            sessionData[fd].mailTo.push_back(mailAddr);
            SMTP::SendCommandResponse(sock, 250);
            break;
        }
        case SMTP_MAIL_DATA_START:
        {
            SMTP::SendCommandResponse(sock, 354);
            sessionStatus[fd] = SMTP_MAIL_DATA;
            break;
        }
        case SMTP_MAIL_DATA:
        {
            sessionData[fd].data += data;
            if (sessionData[fd].data.find("\r\n.\r\n") != string::npos)
            {
                sessionStatus[fd] = SMTP_GREETINGS;
                Files::AsciiFile file(sessionData[fd].mailTo[0]);
                file.writeFile(sessionData[fd].data);
                file.SaveToFile();
                sessionData[fd].data.clear();
                sessionData[fd].mailTo.clear();
                SMTP::SendCommandResponse(sock, 250);
            }
            break;
        }
        
        case SMTP_QUIT: // Client quit
            app->socketMgr->CloseSocketByFd(fd);
        break;
        default:
            SMTP::SendCommandResponse(sock, 550);
        break;
    }
}
