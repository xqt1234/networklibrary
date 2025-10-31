#pragma once
#include "CallBacks.h"
#include "InetAddress.h"
#include "Socket.h"
class Acceptor
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress& addr)>;
private:
    Socket m_socket;
    NewConnectionCallback m_NewConnectionCallback;
    EventCallBack m_test;
    bool m_listenning{false};
public:
    Acceptor(InetAddress listenaddr);
    ~Acceptor();
    void listen();
    void setConnectionCallBack(const NewConnectionCallback& cb)
    {
        m_NewConnectionCallback = cb;
    }
    void setTestCallBack(const EventCallBack& cb)
    {
        m_test = cb;
    }
};