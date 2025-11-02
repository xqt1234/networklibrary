#include "TcpServer.h"
#include "Acceptor.h"
#include <functional>
#include <iostream>
#include "Logger.h"
#include "EventLoop.h"
TcpServer::TcpServer(EventLoop* loop,uint16_t port, string addr)
    : m_acceptor(new Acceptor(InetAddress(port, addr)))
    , m_loop(loop)
    , m_pool(new EventLoopThreadPoll(loop))
{
    
    m_acceptor->setConnectionCallBack(std::bind(&TcpServer::newConnection, this, _1, _2));
    // m_acceptor->setTestCallBack(std::bind(&TcpServer::testRecv, this));
}

TcpServer::~TcpServer()
{
}

void TcpServer::newConnection(int connfd, const InetAddress &peerAddr)
{
    std::cout << "有新连接到达" << peerAddr.toIpPortString() << std::endl;
    m_clients.emplace(connfd, peerAddr);
    EventLoop* ioLoop = m_pool->getNextLoop();
    
}

void TcpServer::start()
{
    m_loop->runInLoop(std::bind(&Acceptor::listen,m_acceptor.get()));
    m_pool->start();
}

void TcpServer::testRecv()
{
    auto it = m_clients.begin();
    while (it != m_clients.end())
    {
        char buf[1024];
        int len = recv(it->first, buf, sizeof(buf), MSG_DONTWAIT);
        if (len > 0)
        {
            buf[len] = '\0';
            std::cout << std::string(buf) << std::endl;
            send(it->first, buf, len, 0);
            ++it;
        }
        else if (len == 0)
        {
            LOG_INFO("客户端断开连接");
            close(it->first);
            it = m_clients.erase(it);
        }
        else
        {
            int err = errno;
            if (err == EAGAIN || err == EINTR)
            {
                ++it;
                continue;
            }
            LOG_ERROR("收数据错误");
            close(it->first);
            it = m_clients.erase(it);
        }
    }
}

void TcpServer::setThreadNum(int num)
{
    m_pool->threadnum(num);
}
