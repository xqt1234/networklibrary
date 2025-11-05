#include "Socket.h"
#include <unistd.h>
#include "Logger.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
Socket::Socket()
{
    
    m_sockfd = ::socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if(m_sockfd < 0)
    {
        LOG_FATAL("创建socket错误");
    }else
    {
        LOG_INFO("创建socket成功{}",m_sockfd);
    }
}

Socket::~Socket()
{
    close(m_sockfd);
}

int Socket::accept(InetAddress *peerAddr)
{
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    int connfd = ::accept4(m_sockfd,(sockaddr*)&addr,&len,SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd < 0)
    {
        
        int savedErrno = errno;
        switch (savedErrno)
        {
        case EAGAIN:       // 或EWOULDBLOCK: 非阻塞socket没有连接 pending
        case ECONNABORTED: // 连接已中止（客户端在完成前关闭）
        case EINTR:        // 被信号中断
            // 这些都是正常情况，应该继续运行
            break;
        case EMFILE: // 文件描述符耗尽
            // 特殊处理，但不应该终止程序
            break;
        case EPERM:
        case ENOBUFS:
        case ENOMEM:
            // 资源限制，但程序应该继续服务现有连接
            break;
        default:
            // 只有真正严重的错误才考虑终止
            LOG_FATAL("unexpected accept error");
            // 但即使这样，也要根据情况决定是否终止
        }
    }
    peerAddr->setAddr(addr);
    return connfd;
}

void Socket::bindAddress(InetAddress listenAddr)
{
    int ret = ::bind(m_sockfd,(sockaddr *)listenAddr.getSockAddr(), sizeof(sockaddr_in));
    if (ret == -1)
    {
        LOG_FATAL("绑定失败{}",listenAddr.toIpPortString());
    }
    LOG_INFO("绑定成功{}",listenAddr.toIpPortString());
}

void Socket::listen()
{
    if(0 != ::listen(m_sockfd,1024))
    {
        LOG_FATAL("监听失败");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
}

void Socket::shutdownWrite()
{

}
