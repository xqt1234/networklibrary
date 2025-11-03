#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
TcpConnection::TcpConnection(EventLoop* loop,int cfd, InetAddress address)
    : m_fd(cfd)
    , m_loop(loop)
    , m_addr(address)
    , m_channel(new Channel(cfd))
{
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::connectEstablished()
{
    m_channel->tie(shared_from_this());
    m_channel->enableReading();
    m_ConnectionCallBack(shared_from_this());
}
