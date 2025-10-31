#include "TcpServer.h"
#include "Acceptor.h"
#include <functional>
#include <iostream>
#include "Logger.h"
TcpServer::TcpServer(uint16_t port, string addr)
    : m_acceptor(new Acceptor(InetAddress(port, addr)))
{
    m_acceptor->setConnectionCallBack(std::bind(&TcpServer::newConnection, this, _1, _2));
    m_acceptor->setTestCallBack(std::bind(&TcpServer::testRecv, this));
}

TcpServer::~TcpServer()
{
}

void TcpServer::newConnection(int connfd, const InetAddress &peerAddr)
{
    std::cout << "有新连接到达" << peerAddr.toIpPortString() << std::endl;
    m_clients.emplace(connfd, peerAddr);
}

void TcpServer::start()
{
    m_acceptor->listen();
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
