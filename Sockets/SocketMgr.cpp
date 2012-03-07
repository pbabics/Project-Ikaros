#include "SocketMgr.h"

ConnectionMgr::~ConnectionMgr()
{
    CloseAllSockets();
}

void ConnectionMgr::CloseAllSockets()
{
    while (sockets.size())
    {
        SocketMap::iterator it = sockets.begin();
        it->second->Close();
        sockets.erase(it);
    }
}

void ConnectionMgr::CloseAllPendingSockets()
{
    while (queue.size())
    {
        queue.front()->second->Close();
        sockets.erase(queue.front());
        queue.pop();
    }
}

Socket* ConnectionMgr::AddSocket(Socket* sock)
{
    if (sockets.find(sock->GetFD()) != sockets.end())
        return sock;

    sockets.insert(std::pair<int, Socket*>(sock->GetFD(), sock));
    return sock;
}

void ConnectionMgr::CloseSocketById(uint32 id)
{
    if (id > sockets.size())
        return;

    SocketMap::iterator sock = GetSocketMapElementById(id);
    if (sock == sockets.end())
        return;

    if (closeImmediatly)
    {
        sock->second->Close();
        sockets.erase(sock);
    }
    else
        queue.push(sock);
}

void ConnectionMgr::CloseSocketByFd(int fd)
{
    SocketMap::iterator sock = sockets.find(fd);
    if (sock == sockets.end())
        return;

    if (closeImmediatly)
    {
        sock->second->Close();
        sockets.erase(sock);
    }
    else
        queue.push(sock);
}

void ConnectionMgr::CloseSocket(SocketMap::iterator sock)
{
    if (closeImmediatly)
    {
        sock->second->Close();
        sockets.erase(sock);
    }
    else
        queue.push(sock);
}

Socket* ConnectionMgr::GetSocketById(uint32 id)
{
    if (id > sockets.size())
        return sockets.begin()->second;
    SocketMap::iterator sock = GetSocketMapElementById(id);

    return sock->second;
}

Socket* ConnectionMgr::GetSocketByFd(int fd)
{
    SocketMap::iterator it = sockets.find(fd);
    if (it != sockets.end())
    {
        return it->second;
    }
    return NULL;
}

ConnectionMgr::SocketMap::iterator ConnectionMgr::GetSocketMapElementById(uint32 id)
{
    if (id > sockets.size())
        return sockets.end();
    SocketMap::iterator it = sockets.begin();
    advance(it,id);
    return it;
}

int ConnectionMgr::GetHighestFd()
{
    if (sockets.size()  == 0)
        return 0;

    return GetSocketMapElementById(sockets.size()-1)->second->GetFD();
}
