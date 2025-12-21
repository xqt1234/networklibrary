#include "Connector.h"
#include "Logger.h"
#include "EventLoop.h"
#include "SocketTools.h"
using namespace mymuduo;
Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
    : m_serverAddr(serverAddr), m_loop(loop)
{
}

Connector::~Connector()
{
    if (m_channel)
    {
        auto ch = std::move(m_channel);
        auto ch_shared = std::shared_ptr<Channel>(std::move(ch));
        m_loop->runInLoop([ch_shared]() {
            ch_shared->disableAll();
            ch_shared->remove();
            close(ch_shared->fd());
        });
    }
}

void Connector::connect()
{
    if (m_channel)
    {
        auto ch = std::move(m_channel);
        auto ch_shared = std::shared_ptr<Channel>(std::move(ch));
        m_loop->runInLoop([ch_shared]() {
            ch_shared->disableAll();
            ch_shared->remove();
            close(ch_shared->fd());
        });
    }
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0)
    {
        LOG_FATAL("创建socket错误");
    }
    else
    {
        LOG_INFO("创建socket成功{}", sockfd);
    }
    const sockaddr_in *addr = m_serverAddr.getSockAddr();
    int ret = ::connect(sockfd, (sockaddr *)addr, sizeof(sockaddr_in));
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        connecting(sockfd);
        break;
    default:
        LOG_ERROR("连接服务器失败{}", m_serverAddr.toIpPortString());
        close(sockfd);
        break;
    }
}

void Connector::connecting(int sockfd)
{
    m_state = States::kConnecting;
    m_channel.reset(new Channel(sockfd, m_loop));
    m_channel->setWriteCallBack(std::bind(&Connector::handleWrite, this));
    m_channel->setErrorCallBack(std::bind(&Connector::handleError, this));
    m_channel->enableWriting();
}

void Connector::handleWrite()
{
    if (m_state == States::kConnecting)
    {
        int fd = removeAndResetChannel();
        int sockeErro = getSocketError(fd);
        if (sockeErro != 0 || isSelfConnect(fd))
        {
            LOG_ERROR("连接错误{}", m_serverAddr.toIpPortString());
            close(fd);
            //m_retryCallBack();
        }
        else
        {
            m_state = States::kConnected;
            m_NewConnectionCallback(fd);
        }
    }
}

void Connector::handleError()
{
    if (m_state == States::kConnecting)
    {
        int sockfd = removeAndResetChannel();
        if (sockfd > 0)
        {
            int err = getSocketError(sockfd);
            LOG_ERROR("连接出错了{}", err);
            close(sockfd);
        }
        m_retryCallBack();
    }
}

void Connector::resetChannel()
{
    m_channel.reset();
}

int Connector::removeAndResetChannel()
{
    if (m_channel == nullptr)
    {
        return -1;
    }
    m_channel->disableAll();
    int fd = m_channel->fd();
    m_channel->remove();
    m_loop->queueInLoop(std::bind(&Connector::resetChannel, this));
    return fd;
}
