#include "TcpClient.h"
#include "TcpConnection.h"
#include <iostream>
using namespace mymuduo;
const int TcpClient::kInitRetryMs = 500;
const int TcpClient::kMaxRetryMs = 30 * 1000;
TcpClient::TcpClient(EventLoop *loop, const InetAddress &addr, std::string name)
    : m_loop(loop)
    , m_addr(addr)
    , m_name(name)
    , m_connector(new Connector(loop,addr))
    , m_currentRetryMs(kInitRetryMs)
    , m_connection()
{
    m_connector->setNewConnectionCallback(std::bind(&TcpClient::handNewConnection,this,std::placeholders::_1));
    m_connector->setRetryCallBack(std::bind(&TcpClient::retryConnection,this));
}

TcpClient::~TcpClient()
{
    m_retry = false;
    if(m_timerid.hasSet())
    {
        m_loop->cancelTimer(m_timerid);
    }
    disconnect();
}

void TcpClient::connect(bool retry)
{
    m_retry = retry;
    m_loop->runInLoop([this]{
        m_connector->connect();
    });
}

void TcpClient::disconnect()
{
    m_retry = false;
    std::lock_guard<std::mutex> lock(m_conMtx);
    if(m_connection)
    {
        m_connection->shutdown();
    }
}


TcpConnectionPtr TcpClient::connection()
{
    if(!m_isconnect)
    {
        return nullptr;
    }
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
    m_isconnect = true;
    conn->connectEstablished();
    
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    m_isconnect = false;
    {
        std::lock_guard<std::mutex> lock(m_conMtx);
        m_connection.reset();
    }
    m_loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
    retryConnection();
}

void TcpClient::retryConnection()
{
    if(m_retry && !m_isconnect)
    {
        m_timerid = m_loop->runAfter([this]{
            m_connector->connect();
        },m_currentRetryMs/1000.0);
        m_currentRetryMs = m_currentRetryMs * 2;
        m_currentRetryMs = std::min(kMaxRetryMs,m_currentRetryMs);
    }
}
