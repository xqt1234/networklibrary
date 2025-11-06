#pragma once
#include "CallBacks.h"
#include "InetAddress.h"
#include "Socket.h"
class Channel;
class EventLoop;
class Acceptor
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress& addr)>;
private:
    Socket m_socket;
    NewConnectionCallback m_NewConnectionCallback;
    // EventCallBack m_test;
    bool m_listenning{false};
    Channel* m_channel;
    EventLoop* m_loop;
public:
    Acceptor(EventLoop* loop,InetAddress listenaddr);
    ~Acceptor();
    void listen();
    void setConnectionCallBack(const NewConnectionCallback& cb)
    {
        m_NewConnectionCallback = cb;
    }
    void handleRead();
    int fd(){return m_socket.fd();}
    
    // void setTestCallBack(const EventCallBack& cb)
    // {
    //     m_test = cb;
    // }
};