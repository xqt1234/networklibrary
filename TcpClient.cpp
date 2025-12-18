#include "TcpClient.h"
#include "TcpConnection.h"
#include <iostream>
using namespace mymuduo;
TcpClient::TcpClient(EventLoop *loop, const InetAddress &addr, std::string name)
    : m_loop(loop)
    , m_addr(addr)
    , m_name(name)
    , m_connector(new Connector(loop,addr))
{
    m_connector->setNewConnectionCallback(std::bind(&TcpClient::handNewConnection,this,std::placeholders::_1));
}

TcpClient::~TcpClient()
{
    disconnect();
}

void TcpClient::connect()
{
    m_connector->start();
}

void TcpClient::disconnect()
{
    std::lock_guard<std::mutex> lock(m_conMtx);
    if(m_connection)
    {
        m_connection->shutdown();
    }
}

TcpConnectionPtr TcpClient::connection()
{
    std::lock_guard<std::mutex> lock(m_conMtx);
    return m_connection;
}

void TcpClient::handNewConnection(int sockfd)
{
    TcpConnectionPtr conn(new TcpConnection(m_loop, sockfd, m_addr));
    static int m_nextConnId = 0;
    conn->setConnId(m_nextConnId++);
    m_nextConnId++;
    conn->setConnectionCallBack(m_ConnectionCallBack);
    conn->setMessageCallback(m_MessageCallBack);
    conn->setCloseCallBack(std::bind(&TcpClient::removeConnection, this, _1));
    {
        std::lock_guard<std::mutex> lock(m_conMtx);
        m_connection = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    {
        std::lock_guard<std::mutex> lock(m_conMtx);
        m_connection.reset();
    }
    m_loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
}
