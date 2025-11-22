#include <iostream>
#include "TcpServer.h"
#include "EventLoop.h"
#include "Channel.h"
#include "TcpConnection.h"
#include <functional>
#include "Logger.h"
#include "HttpResponse.h"
#include <signal.h>
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
        m_server.setMessageCallBack(std::bind(&EchoServer::onHttpMessage, this, _1, _2));
    }
    void newConnection(const TcpConnectionPtr &conn)
    {
        if (conn->isConnected())
        {
            //std::cout << "启用外面的回调" << std::endl;
        }
        else
        {
            //std::cout << "断开连接" << std::endl;
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
        if (!msg.empty() && msg.back() == '\n')
        {
            msg.pop_back();
        }
        if (!msg.empty() && msg.back() == '\r')
        {
            msg.pop_back();
        }
        std::cout << msg << std::endl;
        conn->send(msg + "aaaaa\n");
        // conn->shutdown();
    }
    void onHttpMessage(const TcpConnectionPtr &conn, Buffer *buf)
    {
        std::string request = buf->readAllAsString();
        std::string method, path;
        std::istringstream iss(request);
        iss >> method >> path;
        std::string res = HttpResponse::getInstance().getFile("index.html");
        //std::string res = HttpResponse::getInstance().getCacheFile("index.html");
        conn->send(res);
    }
};

int main()
{
    signal(SIGPIPE, SIG_IGN);
    Logger::getInstance().setLogLevel(LogLevel::DEBUG);
    EventLoop loop;
    EchoServer server(&loop, 9999, "192.168.65.4");
    server.setThreadNum(4);
    server.start();
    loop.loop();
    std::cout << "hello world" << std::endl;
    return 0;
}