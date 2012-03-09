#include "Socket.h"


Socket::~Socket()
{
    Close();
}

int Socket::Close()
{
    if (socket && isConnected)
        return close(socket);
    return 0;
}

int Socket::Send(const void* buffer, size_t len, int flags)
{
    size_t sended = 0;
    int ret = 0;
    while (len > sended)
    {
        if (!this || socket < 0)
            return -1;
        #pragma GCC diagnostic ignored "-Wpointer-arith"
        ret = send(socket, (const void*) (buffer + sended), len - sended, flags);
        #pragma GCC diagnostic warning "-Wpointer-arith"
        if (ret > 0)
        {
            sended += ret;
            //printf("\n\nSending %lu bytes (Already Sended:  %lu bytes   %f%) ret: %d  errno %d\n\n", len, sended, sended  * 100.f/ float(len), ret, errno);
        }
        else if (ret == 0)
        {
            isConnected = false;
            error = errno;
            return ret;
        }
        else if (ret == -1)
        {
            error = errno;
            switch (error)
            {
                case EAGAIN:
                    break;
                default:
                    return ret;
            }
        }
    }
    return sended;
}

int Socket::Send(BinnaryData &data, int flags)
{
    return Send(data, data.size(), flags);
}

int Socket::Recieve(void* buffer, size_t len, int flags)
{
    int ret = 0;
    ret = recv(socket, buffer, len, flags);
    if (ret == 0)
    {
        isConnected = false;
        error = errno;
    }
    if (ret == -1)
        error = errno;
    return ret;
}

int Socket::Recieve(BinnaryData &data, int flags)
{
    return Recieve(data, data.capacity(), flags);
}


bool _isIPv4(char* ip)
{
    int Byte[4];
    short pos[5]={-1, 0 ,0, 0, 0};
    if (strlen(ip) - 1 < 7)
        return false;
    if (strlen(ip) - 1 > 15)
        return false;
    ushort i = 0;
    ushort dotc=0;

    while(i != strlen(ip) - 1)
    {
        if (*(ip+i) == '.')
        dotc++;
        i++;
    }

    if (dotc != 3)
        return false ;
    for (uint i = 0, posc = 1; i < strlen(ip) - 1; i++)
    {
        if (*(ip + i) == '.')
        {
            if(posc == 5)
                return false;
            pos[posc] = i; 

            if (abs(pos[posc - 1] - pos[posc]) - 1 > 3)
                return false;
            Byte[posc]= atoi(ip + pos[posc] + 1);
            posc++;
        }
    }
    Byte[0] = atoi(ip);

    for (ushort i = 0; i < 4; i++)
    {
        if (Byte[i] < 0 || Byte[i] > 255)
            return false;
    }
    return true;
}


int TCPSocket::Send(const char *buff, unsigned short length, unsigned char flags)
{
    int ret = send(sock, buff, length, flags);
    isConnected = !(ret == 0);
    return ret;
}

int TCPSocket::Recv(char *buff, unsigned short length, unsigned char flags)
{
    int ret = recv(sock, buff, length, flags);
    isConnected = !(ret == 0);
    return ret;
}

int TCPSocket::Connect(char* Ip)
{
    char* port = strchr(Ip, ':') + 1 ;
    char ip[strlen(Ip) + 5];
    memset(ip, 0, strlen(Ip) + 5);
    strcpy(ip, Ip);
    *((char*) strrchr(ip, ':')) = '\0';
    if (_isIPv4(ip))
        return Connect(ip, port);
    else 
        return DNSConnect(ip, port);
}

int TCPSocket::Connect(char *IPAdress, char *Port)
{
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(Port));
    dest.sin_addr.s_addr = inet_addr(IPAdress);
    if (_isIPv4(IPAdress))
    {
        int ret = connect(sock, (struct sockaddr*)&dest, sizeof(dest));
        isConnected = (ret == 0);
        return ret;
    }
    else
        return DNSConnect(IPAdress, Port);
}

int TCPSocket::Connect(unsigned int IPAdress, unsigned short Port)
{
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = Port;
    dest.sin_addr.s_addr = IPAdress;

    int ret = connect(sock, (struct sockaddr*)&dest, sizeof(dest));
    isConnected = (ret == 0);
    return ret;
}

int TCPSocket::Connect(char* IPAdress, unsigned short Port)
{
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(Port);
    dest.sin_addr.s_addr = inet_addr(IPAdress);
    if (_isIPv4(IPAdress))
    {
        int ret = connect(sock, (struct sockaddr*)&dest, sizeof(dest));
        isConnected = (ret == 0);
        return ret;
    }
    else
        return DNSConnect(IPAdress, Port);
}

int TCPSocket::DNSConnect(char *DomainName, char *Port)
{
    struct hostent* host ;
    host = gethostbyname(DomainName);

    if (!host)
        return -1 ;
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(Port));
    dest.sin_addr.s_addr= *((unsigned long *) host->h_addr_list[0]);
    
    int ret = connect(sock, (struct sockaddr*)&dest, sizeof(dest));
    isConnected = (ret == 0);
    return ret;
}

int TCPSocket::DNSConnect(char *DomainName, unsigned short Port)
{
    struct hostent* host ;
    host = gethostbyname(DomainName);

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(Port);
    dest.sin_addr.s_addr= *((unsigned long *) host->h_addr_list[0]);

    int ret = connect(sock, (struct sockaddr*)&dest, sizeof(dest));
    isConnected = (ret == 0);
    return ret;
}

TCPSocket::TCPSocket()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
}

TCPSocket::~TCPSocket()
{
}
