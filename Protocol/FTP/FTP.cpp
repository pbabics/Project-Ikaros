#include "FTP.h"


void FTP::SendGreetings(Socket* sock)
{
    stringstream stream;
    stream << "220 Server-Ikaros " << stringConfigs[CONFIG_STRING_GREETINGS] << newline;
    sock->Send(stream);
}

void FTP::SendHelloResponse(Socket* sock, std::string name)
{
    stringstream stream;
    stream << "250 Hello "<< name << newline;
    sock->Send(stream);
}

void FTP::SendCommandResponse(Socket* sock, int responseCode)
{
    if (responseCodeMessage.find(responseCode) == responseCodeMessage.end())
        return;

    stringstream stream;
    stream << responseCode << ' ' << responseCodeMessage[responseCode] << newline;
    sock->Send(stream);
}

void FTP::SendSpecialCommandResponse(Socket* sock, int responseCode, string text)
{
    if (responseCodeMessage.find(responseCode) == responseCodeMessage.end())
        return;

    stringstream stream;
    stream << responseCode << ' ' << text << newline;
    sock->Send(stream);
}

void* SendData(void* args)
{
    protoLog->outDebug("SendData Begin");
    SessionSendStruct* sendStat = (SessionSendStruct*) args;
    Socket* sock = sendStat->so;
    SessionDataStruct& session = sendStat->s;
    char* data = sendStat->d;
    size_t dataLength = sendStat->l;
    FTP::SendCommandResponse(sock, 150);

    int sendReturn = 0;
    uint64 sleepTime = intConfigs[CONFIG_INT_SEND_WAIT_TIME], errSleepTime = intConfigs[CONFIG_INT_SEND_ERROR_WAIT_TIME];
    size_t sended = 0, sendingPart = intConfigs[CONFIG_INT_MAX_DATA_SEGMENT_SIZE];
    bool interrupt = false;

    if (!session.isPassive)
    {
        TCPSocket* connection = new TCPSocket();
        int i = connection->Connect(session.ip.ToChar(), session.port);
        if (!connection->Connected())
        {
            FTP::SendCommandResponse(sock, 425); // Cannot open connection
            session.DTPActive = false;
            session.activeSend = NULL;
            session.abortTranfser = false;
            protoLog->outDebug("Cannot connect to:  %s:%d connection: %d", session.ip.ToChar(), session.port, i);
            delete connection;
            delete data;
            delete sendStat;
            return NULL;
        }

        while (!session.abortTranfser && connection->Connected() && !interrupt)
        {
            sendReturn = connection->Send(data + sended, dataLength - sended > sendingPart ? sendingPart : dataLength - sended, 0 );
            //protoLog->outDebug("Sended:  %d bytes from %lu remaining:  %lu", sendReturn, dataLength, dataLength - sended);
            if (sendReturn == -1)
            {
                if (errno != EAGAIN)
                    break;
                usleep(errSleepTime);
                continue;
            }
            if (sendReturn == 0)
            {
                interrupt = true;
                break;
            }
            if (sendReturn > 0)
                sended += sendReturn;
            if ((dataLength - sended == 0))
                break;
            if (sleepTime)
                usleep(sleepTime);
        }
        connection->Close();
        delete connection;
    }
    else
    {
        int& lSock = session.passiveSock;

        if (lSock < 0)
        {
            FTP::SendCommandResponse(sock, 425);
            session.DTPActive = false;
            session.activeSend = NULL;
            session.abortTranfser = false;
            lSock = close(lSock);
            delete data;
            delete sendStat;
            return NULL;
        }

        struct sockaddr_in peer;
        unsigned int peerSize = sizeof(peer);
        int newSocket = accept(lSock, (sockaddr*)&peer, &peerSize);
        Socket* pSock = new Socket(newSocket, peer);

        while (!session.abortTranfser && !interrupt)
        {
            sendReturn = pSock->Send(data + sended, dataLength - sended > sendingPart ? sendingPart : dataLength - sended, 0 );
            //protoLog->outDebug("Sended:  %d bytes from %lu remaining:  %lu", sendReturn, dataLength, dataLength - sended);
            if (sendReturn == -1)
            {
                if (errno != EAGAIN)
                    break;
                usleep(errSleepTime);
                continue;
            }
            if (sendReturn == 0)
            {
                interrupt = true;
                break;
            }
            if (sendReturn > 0)
                sended += sendReturn;
            if ((dataLength - sended == 0))
                break;
            if (sleepTime)
                usleep(sleepTime);
        }
        pSock->Close();
        lSock = close(lSock);
        delete pSock;
    }

    if (!interrupt)
        FTP::SendCommandResponse(sock, 226);
    if (session.abortTranfser)
        FTP::SendCommandResponse(sock, 426);
    delete data;
    delete sendStat;
    session.DTPActive = false;
    session.activeSend = NULL;
    session.abortTranfser = false;
    protoLog->outDebug("SendData End session: %s", session.DTPActive? "Data Tranfsering": "Data Tranfser Completed" );
    return NULL;
}

void* RecvData(void* args)
{
    protoLog->outDebug("RecvData Begin");
    SessionRecvStruct* recvStat = (SessionRecvStruct*) args;
    Socket* sock = recvStat->so;
    SessionDataStruct& session = recvStat->s;
    const char* fileName = recvStat->f;
    FTP::SendCommandResponse(sock, 150);

    int recvReturn = 0;
    uint64 sleepTime = intConfigs[CONFIG_INT_RECV_WAIT_TIME], errSleepTime = intConfigs[CONFIG_INT_RECV_ERROR_WAIT_TIME];
    size_t recieved = 0, recievePart = intConfigs[CONFIG_INT_MAX_DATA_SEGMENT_SIZE];
    bool interrupt = false;
    char recievedData[recievePart + 1];

    Files::BinFile file;
    file.open(fileName, ios_base::trunc | ios_base::binary | ios_base::out);
    if (file.is_open())
    {
        if (!session.isPassive)
        {
            TCPSocket* connection = new TCPSocket();
            int i = connection->Connect(session.ip.ToChar(), session.port);
            if (!connection->Connected())
            {
                FTP::SendCommandResponse(sock, 425); // Cannot open connection
                session.DTPActive = false;
                session.activeRecv = NULL;
                session.abortTranfser = false;
                protoLog->outDebug("Cannot connect to:  %s:%d connection: %d", session.ip.ToChar(), session.port, i);
                delete connection;
                delete recvStat;
                return NULL;
            }


            while (!session.abortTranfser && connection->Connected() && !interrupt)
            {
                memset(recievedData, 0, recievePart + 1);
                recvReturn = connection->Recv(recievedData, recievePart, 0 );
                if (recvReturn == -1)
                {
                    if (errno != EAGAIN)
                        break;
                    usleep(errSleepTime);
                    continue;
                }
                if (recvReturn == 0)
                {
                    interrupt = true;
                    break;
                }
                if (recvReturn > 0)
                {
                    recieved += recvReturn;
                    file.write(recievedData, recvReturn);
                }
                if (sleepTime)
                    usleep(sleepTime);
            }
            connection->Close();
            file.close();
            delete connection;
        }
        else
        {
            int& lSock = session.passiveSock;

            if (lSock < 0)
            {
                FTP::SendCommandResponse(sock, 425);
                session.DTPActive = false;
                session.activeRecv = NULL;
                session.abortTranfser = false;
                lSock = close(lSock);
                delete recvStat;
                return NULL;
            }

            struct sockaddr_in peer;
            unsigned int peerSize = sizeof(peer);
            int newSocket = accept(lSock, (sockaddr*)&peer, &peerSize);
            Socket* pSock = new Socket(newSocket, peer);

            while (!session.abortTranfser && !interrupt)
            {
                memset(recievedData, 0, recievePart + 1);
                recvReturn = pSock->Recieve(recievedData, recievePart, 0 );
                if (recvReturn == -1)
                {
                    if (errno != EAGAIN)
                        break;
                    usleep(errSleepTime);
                    continue;
                }
                if (recvReturn == 0)
                {
                    interrupt = true;
                    break;
                }
                if (recvReturn > 0)
                {
                    recieved += recvReturn;
                    file.write(recievedData, recvReturn);
                }
                if (sleepTime)
                    usleep(sleepTime);
            }
            pSock->Close();
            lSock = close(lSock);
            file.close();
            delete pSock;
        }
    }
    else
        protoLog->outError("Cannot open file '%s' for writing", fileName);

    if (!session.abortTranfser)
        FTP::SendCommandResponse(sock, 226);
    if (session.abortTranfser)
        FTP::SendCommandResponse(sock, 426);

    protoLog->outDebug("RecvData Recieved:  %lu Bytes", recieved);

    delete recvStat;
    session.DTPActive = false;
    session.activeRecv = NULL;
    session.abortTranfser = false;
    protoLog->outDebug("RecvData End session: %s", session.DTPActive? "Data Tranfsering": "Data Tranfser Completed" );
    return NULL;
}

void FTP::SendOverDTP(int fd, char* data, size_t dataLength)
{
    if (!data)
        return;
    SessionDataStruct& session = sessionData[fd];
    Socket* sock = app->socketMgr->GetSocketByFd(fd);
    protoLog->outDebug("Sending %d bytes over DTP to: %s:%d  Mode: %s", dataLength,  session.ip.ToChar(), session.port, session.isPassive? "Passive" : "Active");
    SessionSendStruct* s = new SessionSendStruct(session, sock, data, dataLength);
    session.DTPActive = true;
    session.activeSend = s;
    session.abortTranfser = false;
    Thread::CreateThread(&SendData, (void*) s);
}

void FTP::RecieveOverDTP(int fd, const char* fileName)
{
    if (!fileName)
        return;
    SessionDataStruct& session = sessionData[fd];
    Socket* sock = app->socketMgr->GetSocketByFd(fd);
    protoLog->outDebug("Recv over DTP from: %s:%d  Mode: %s",  session.ip.ToChar(), session.port, session.isPassive? "Passive" : "Active");
    SessionRecvStruct* s = new SessionRecvStruct(session, sock, fileName);
    session.DTPActive = true;
    session.activeRecv = s;
    session.abortTranfser = false;
    Thread::CreateThread(&RecvData, (void*) s);
}

void FTP::InitPassiveSock(SessionDataStruct& session)
{
    int& sock = session.passiveSock;
    close(sock);
    int r = 0;
    struct sockaddr_in dest;
    sock = socket(AF_INET, SOCK_STREAM, 0) ;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sock, sizeof sock);

    if (sock < 0)
    {
        protoLog->outError("DTP Passive Socket Creation failed. Error: %d", errno);
    }

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = 0;
    dest.sin_addr.s_addr = inet_addr(app->StringConfigs[CONFIG_STRING_BIND_IP].c_str());
    //Bind
    r = bind(sock, (struct sockaddr*)&dest, sizeof(dest));
    if (r == -1)
    {
        protoLog->outError("DTP Passive Socket Bind Returned Error: %d", errno);
        close(sock);
    }

    //Listen
    r = listen(sock, 2);
    if (r == -1)
    {
        protoLog->outError("DTP Passive Socket Listen Returned Error: %d", errno);
        close(sock);
    }
    else
    {
        struct sockaddr_in info;
        bzero(&info, sizeof(info));
        socklen_t len = sizeof (info);
        if (getsockname(sock, (struct sockaddr*)&info, &len) == 0)
        {
            session.isPassive = true;
            session.port = ntohs(info.sin_port);
            session.ip = IP(info.sin_addr);
        }
        else
        {
            session.isPassive = false;
            session.port = 0;
        }
    }
}
