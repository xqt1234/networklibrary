#include <iostream>
#include "TcpServer.h"
#include "EventLoop.h"
#include "Channel.h"
#include "TcpConnection.h"
#include <functional>
class EchoServer
{
private:
    TcpServer m_server;
    EventLoop *m_loop;
public:
    EchoServer(EventLoop* loop,uint16_t port,std::string ipaddr)
        :m_loop(loop)
        ,m_server(loop,port,ipaddr)
    {
        m_server.setConnectionCallBack(std::bind(&EchoServer::newConnection,this,_1));
    }
    void newConnection(const TcpConnectionPtr& conn)
    {
        std::cout << "启用外面的回调" << std::endl;
    }
    void start()
    {
        m_server.start();
    }
    void setThreadNum(int num)
    {
        m_server.setThreadNum(num);
    }
};

int main()
{
    EventLoop loop;
    EchoServer server(&loop,9999, "0.0.0.0");
    server.setThreadNum(2);
    server.start();
    std::cout << "hello world" << std::endl;
    return 0;
}