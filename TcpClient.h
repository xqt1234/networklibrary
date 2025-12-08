#pragma once
#include "EventLoop.h"
#include "InetAddress.h"
#include "Connector.h"
#include "CallBacks.h"
#include <mutex>
#include "TcpConnection.h"
class TcpClient
{
private:
    EventLoop* m_loop;
    InetAddress m_addr;
    std::string m_name;
    bool m_isconnect;
    std::unique_ptr<Connector> m_connector;
    ConnectionCallBack m_ConnectionCallBack;
    MessageCallBack m_MessageCallBack;
    TcpConnectionPtr m_connection;
    std::mutex m_conMtx;
public:
    TcpClient(EventLoop* loop,const InetAddress& addr,std::string name);
    ~TcpClient();
    void connect();
    void disconnect();
    void handNewConnection(int sockfd);
    void setConnectionCallBack(const ConnectionCallBack& cb)
    {
        m_ConnectionCallBack = cb;
    }
    void setMessageCallBack(const MessageCallBack& cb)
    {
        m_MessageCallBack = cb;
    }
    void removeConnection(const TcpConnectionPtr& conn);
};
