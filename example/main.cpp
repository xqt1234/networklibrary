#include <iostream>
#include "TcpServer.h"
int main()
{
    TcpServer server(9999,"0.0.0.0");
    server.start();
    std::cout << "hello world" << std::endl;
    return 0;
}