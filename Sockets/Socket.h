#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>   // sprintf etc
#include <string.h>   // sprintf etc

#include <cmath>
#include <cstdio>
#include <cstdlib> // for atoi()
#include <errno.h>  // For errno

#include "file.h"

#ifndef __SOCKET
#define __SOCKET

class Socket
{
    public:
            Socket(int sock, sockaddr_in addr): socket(sock), address(addr) { isConnected = true; }
            Socket(int sock): socket(sock) { isConnected = true; }
            ~Socket();

            int Send(const void* buffer, size_t len, int flags = 0);
            int Send(BinnaryData &data, int flags = 0);
            int Send(std::stringstream &data, int flags = 0) { return Send(data.str().c_str(), data.str().length(), flags); }

            int Recieve(void* buffer, size_t len, int flags = 0);
            int Recieve(BinnaryData &data, int flags = 0);

            int Close();
            inline const char* GetAddress()const { return inet_ntoa(address.sin_addr); }
            inline sockaddr_in GetAddr() const { return address; }

            inline int errnum() const { return error; }
            inline bool is_connected() const { return isConnected; }
            operator int() { return socket; }
            inline int GetFD() const { return socket; }

            int SetSocketOption(int level, int option_name, const void *option_value, socklen_t option_len)
            {
                return setsockopt(socket, level, option_name, option_value, option_len);
            }

    private:
            int socket;
            int error;
            bool isConnected;
            sockaddr_in address;
};

bool _isIPv4(char* ip);

class TCPSocket
{
    public:
        int sock ;

    public:
        TCPSocket();
        TCPSocket(int socket): sock(socket) { }
        ~TCPSocket();

        int Send(const char* buff, unsigned short length, unsigned char flags);
        int Recv(char* buff, unsigned short length, unsigned char flags);

        int DNSConnect(char* DomainName, char* Port);
        int DNSConnect(char* DomainName, unsigned short Port);

        int Connect(char* IPAdress, char* Port);
        int Connect(char* Ip, unsigned short Port);
        int Connect(unsigned int IPAdress, unsigned short Port);
        int Connect(char* Ip);

        int setSockOpt(int level, int option_name, const void *option_value, socklen_t option_len)
        {
            return setsockopt(sock, level, option_name, option_value, option_len);
        }

        void Close() { close(sock); }
        void Disconnect() { Close(); }

        bool Connected() const { return isConnected; }

    private:
        struct sockaddr_in dest;
        bool isConnected;
};

#endif
