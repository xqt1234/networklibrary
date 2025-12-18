#include <iostream>
#include "TcpServer.h"
#include "EventLoop.h"
#include "Channel.h"
#include "TcpConnection.h"
#include <functional>
#include "Logger.h"
#include <signal.h>
#include "TcpClient.h"
void onConnection(const TcpConnectionPtr& conn)
{
    std::cout << "被调用" << std::endl;
    if(conn->isConnected())
    {
        std::cout << "建立连接" << std::endl;
    }else
    {
        std::cout << "断开连接" << std::endl;
    }
}
int main()
{
    EventLoop loop;
    std::thread loopthread([&loop]{
        std::cout << "Loop线程启动" << std::endl;
        loop.loop();
        std::cout << "Loop线程结束" << std::endl;
    });
    
    InetAddress addr(10000,"192.168.105.2");
    TcpClient client(&loop,addr,"test");
    client.setConnectionCallBack(&onConnection);
    client.connect();
    std::cout << "hello world" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    if(client.connection() != nullptr && client.connection()->isConnected())
    {
        std::cout << "连接成功" << std::endl;
    }else
    {
        std::cout << "连接失败" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    loop.quit();
    loopthread.join();
    return 0;
}