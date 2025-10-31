#include "Acceptor.h"
#include <iostream>
#include <unordered_set>
#include "Logger.h"
#include <unistd.h>
Acceptor::Acceptor(InetAddress listenaddr)
{
    m_socket.setReuseAddr(true);
    m_socket.setReusePort(true);
    m_socket.bindAddress(listenaddr);
}

Acceptor::~Acceptor()
{
}

void Acceptor::listen()
{
    m_listenning = true;
    m_socket.listen();
    InetAddress addr;

    std::unordered_set<int> clients;
    while (1)
    {
        int cfd = m_socket.accept(&addr);
        if (cfd > 0)
        {
            if(m_NewConnectionCallback)
            {
                m_NewConnectionCallback(cfd,addr);
            }
        }
        if(m_test)
        {
            m_test();
        }
        ::usleep(10000);
    }
}