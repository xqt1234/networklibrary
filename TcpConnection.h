#pragma once
#include <memory>
#include "InetAddress.h"
#include "CallBacks.h"
class EventLoop;
class Channel;
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
private:
    /* data */
    int m_fd;
    InetAddress m_addr;
    std::unique_ptr<Channel> m_channel;
    EventLoop* m_loop;
    ConnectionCallBack m_ConnectionCallBack;
public:
    TcpConnection(EventLoop* loop,int cfd,InetAddress address);
    ~TcpConnection();
    void connectEstablished();
    void setConnectionCallBack(ConnectionCallBack& cb)
    {
        m_ConnectionCallBack = cb;
    }
};
