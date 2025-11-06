#include <iostream>
#include "TcpServer.h"
#include "EventLoop.h"
#include "Channel.h"
#include "TcpConnection.h"
#include <functional>
#include "Logger.h"
class EchoServer
{
private:
    TcpServer m_server;
    EventLoop *m_loop;

public:
    EchoServer(EventLoop *loop, uint16_t port, std::string ipaddr)
        : m_loop(loop), m_server(loop, port, ipaddr)
    {
        m_server.setConnectionCallBack(std::bind(&EchoServer::newConnection, this, _1));
        m_server.setMessageCallBack(std::bind(&EchoServer::onMessage,this,_1,_2));
    }
    void newConnection(const TcpConnectionPtr &conn)
    {
        if (conn->isConnected())
        {
            std::cout << "启用外面的回调" << std::endl;
            Buffer *input = conn->inputBuffer();
            std::string src = input->readAllAsString();
            std::cout << src << std::endl;
            conn->send("hello" + src);
        }else
        {
            std::cout << "断开连接" << std::endl;
            LOG_INFO("连接计数：{}",conn.use_count());
        }
    }
    void start()
    {
        m_server.start();
    }
    void setThreadNum(int num)
    {
        m_server.setThreadNum(num);
    }
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf)
    {
        std::string msg = buf->readAllAsString();
        conn->send(msg + "aaaaa\n");
        conn->shutdown();
    }
};

int main()
{
    EventLoop loop;
    EchoServer server(&loop, 9999, "0.0.0.0");
    server.setThreadNum(2);
    server.start();
    loop.loop();
    std::cout << "hello world" << std::endl;
    return 0;
}