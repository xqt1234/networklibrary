#pragma once
#include <netinet/in.h>
#include <string>
#include <memory>
#include "CallBacks.h"
#include "Acceptor.h"
#include <unordered_map>
#include "EventLoopThreadPoll.h"
using std::string;

class TcpServer
{
public:
    
private:
    std::unique_ptr<Acceptor> m_acceptor;
    ConnectionCallBack m_ConnectionCallBack;
    
    std::unordered_map<int,InetAddress> m_clients;
    EventLoop* m_loop;
    EventLoopThreadPoll* m_pool;
public:
    TcpServer(EventLoop* loop, uint16_t port,string addr = "0.0.0.0");
    ~TcpServer();
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void start();
    void testRecv();
    void setThreadNum(int num);
    void setConnectionCallBack(ConnectionCallBack& cb)
    {
        m_ConnectionCallBack = cb;
    }
};