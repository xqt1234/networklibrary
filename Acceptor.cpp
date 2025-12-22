#include "Acceptor.h"
#include <iostream>
#include <unordered_set>
#include "Logger.h"
#include <unistd.h>
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
using namespace mymuduo;
Acceptor::Acceptor(EventLoop *loop, InetAddress listenaddr)
    : m_socket()
    ,m_channel(m_socket.fd(), loop), m_loop(loop)
{
    m_socket.bindAddress(listenaddr);
    m_socket.setReuseAddr(true);
    m_socket.setReusePort(true);
    m_channel.setReadCallBack(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    m_channel.disableAll();
    m_channel.remove();
    ::close(m_socket.fd());
}

void Acceptor::listen()
{
    m_listenning = true;
    m_socket.listen();
    m_channel.enableReading();
    std::cout << m_channel.events() << std::endl;
}

void Acceptor::handleRead()
{
    InetAddress addr;
    int cfd = m_socket.accept(&addr);
    if (cfd > 0)
    {
        if (m_NewConnectionCallback)
        {
            m_NewConnectionCallback(cfd, addr);
        }
    }
}
