#include <map>
#include <queue>

#include "Socket.h"

using std::map;
using std::queue;

#ifndef __SOCKET_MGR
#define __SOCKET_MGR

class ConnectionMgr
{
    public:
            typedef std::map<int, Socket*> SocketMap;
            typedef std::queue<SocketMap::iterator> CloseQueue;


            ConnectionMgr(): closeImmediatly(false) { sockets.clear(); }
            ~ConnectionMgr();

            Socket* AddSocket(Socket* sock);

            void CloseSocketById(uint32 sock);
            void CloseSocketByFd(int fd);
            void CloseSocket(SocketMap::iterator sock);
            void CloseAllSockets();
            void CloseAllPendingSockets();

            Socket* GetSocketById(uint32 sock);
            SocketMap::iterator GetSocketMapElementById(uint32 id);
            Socket* GetSocketByFd(int fd);

            int GetHighestFd();
            inline int ConnectionsCount() const { return sockets.size(); }

            bool CanCloseImmediatly(bool s) { return closeImmediatly = s; }

            SocketMap sockets;
            CloseQueue queue;
    protected:
            bool closeImmediatly;
};

#endif
