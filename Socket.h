#pragma once
#include "InetAddress.h"
class Socket
{
private:
    int m_sockfd;

public:
    Socket();
    ~Socket();
    int accept(InetAddress *peerAddr);
    void bindAddress(InetAddress listenAddr);
    void listen();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
    void shutdownWrite();
    int fd(){return m_sockfd;}
};