#include "TcpServer.h"
#include "Acceptor.h"
#include <functional>
#include <iostream>
#include "Logger.h"
#include "EventLoop.h"
#include "TcpConnection.h"
TcpServer::TcpServer(EventLoop *loop, uint16_t port, string addr)
    : m_acceptor(new Acceptor(loop, InetAddress(port, addr))), m_loop(loop), m_pool(new EventLoopThreadPoll(loop))
{

    m_acceptor->setConnectionCallBack(std::bind(&TcpServer::newConnection, this, _1, _2));

    // m_acceptor->setTestCallBack(std::bind(&TcpServer::testRecv, this));
}

TcpServer::~TcpServer()
{
    for (auto& val : m_clients)
    {
        TcpConnectionPtr conn(val.second);
        val.second.reset();
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::newConnection(int connfd, const InetAddress &peerAddr)
{
    // std::cout << "有新连接到达" << peerAddr.toIpPortString() << std::endl;
    EventLoop *ioLoop = m_pool->getNextLoop();
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connfd, peerAddr));
    m_clients[m_nextConnId] = conn;
    conn->setConnId(m_nextConnId);
    m_nextConnId++;
    conn->setConnectionCallBack(m_ConnectionCallBack);
    conn->setMessageCallback(m_MessageCallBack);
    conn->setCloseCallBack(std::bind(&TcpServer::removeConnection, this, _1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::start()
{
    m_loop->runInLoop(std::bind(&Acceptor::listen, m_acceptor.get()));
    m_pool->start();
}

void TcpServer::setThreadNum(int num)
{
    m_pool->threadnum(num);
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    // 子线程中调这个，然后，放入主循环中，调用回调
    m_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    // 主循环中会调用这个
    m_clients.erase(conn->getConnId());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    // LOG_DEBUG("在主线程中，主线程loop是{},唤醒的loop是{}", ss.str(),ss2.str());
}
