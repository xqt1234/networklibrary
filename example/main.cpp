#include <iostream>
#include "TcpServer.h"
#include "EventLoop.h"
#include "Channel.h"
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
        
    }
    void newConnection()
    {

    }
};

int main()
{
    EventLoop loop;
    TcpServer server(&loop,9999, "0.0.0.0");
    server.start();
    std::cout << "hello world" << std::endl;
    return 0;
}