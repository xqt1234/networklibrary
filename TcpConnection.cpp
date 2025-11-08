#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Buffer.h"
#include "Socket.h"
TcpConnection::TcpConnection(EventLoop *loop, int cfd, InetAddress address)
    : m_fd(cfd), m_loop(loop), m_addr(address), m_channel(new Channel(cfd, loop)), m_state(StateE::kConnecting), m_highWaterMark(64 * 1024 * 1024), m_socket(new Socket(cfd))
{
    m_channel->setReadCallBack(std::bind(&TcpConnection::handRead, this));
    m_channel->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));
    // if(m_socket)
    // {
    //     m_socket->setTcpNoDelay(true);
    // }
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG("析构了TcpConnection channelfd: {}", m_channel->fd());
    // m_channel->remove();
    // std::cout << "调用m_channel->remove();" << m_channel->fd()<< std::endl;
    // m_channel->remove();
}

void TcpConnection::connectEstablished()
{
    m_state = StateE::kConnected;
    m_channel->tie(shared_from_this());
    m_channel->enableReading();
    m_ConnectionCallBack(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if (m_state == StateE::kConnected)
    {
        m_state = StateE::kDisconnected;
        m_channel->disableAll();
        m_ConnectionCallBack(shared_from_this());
    }
    m_channel->remove();
}

void TcpConnection::send(const std::string& str)
{
    sendInLoop(str.data(), str.length());
}

void TcpConnection::send(const char *data, int len)
{
    if(m_loop->isInLoopThread())
    {
        sendInLoop(data, len);
    }else
    {
        m_loop->runInLoop(std::bind(&TcpConnection::sendInLoop,this,data,len));
    }
    
}

void TcpConnection::shutdown()
{
    if (m_state == StateE::kConnected)
    {
        m_state = StateE::kDisconnecting;
        m_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::sendInLoop(const void *data, size_t len)
{
    if (m_state == StateE::kDisconnected)
    {
        LOG_INFO("连接已关闭，停止发送");
        return;
    }
    size_t remain = len;
    size_t hasSend = 0;
    bool hasError = false;
    if (!m_channel->isWriting() && m_outBuffer.readableBytes() == 0)
    {
        hasSend = ::write(m_fd, data, len);
        if (hasSend >= 0)
        {
            remain = remain - hasSend;
            if (remain == 0 && m_WriteCompleteCallBack)
            {
                m_loop->queueInLoop(std::bind(m_WriteCompleteCallBack, shared_from_this()));
            }
        }
        else
        {
            hasSend = 0;
            if (errno != EWOULDBLOCK)
            {
                LOG_ERROR("发送失败");
            }
            hasError = true;
        }
    }
    if (!hasError && remain > 0)
    {
        // 往buffer的可写的空间写数据后，就是可读数据，即没发出去的数据。
        size_t oldLen = m_outBuffer.readableBytes();
        if (oldLen + remain >= m_highWaterMark && oldLen < m_highWaterMark && m_highWaterMark)
        {
            m_loop->queueInLoop(std::bind(m_HighWaterCallBack, shared_from_this(), oldLen + remain));
        }
        m_outBuffer.append(static_cast<const char *>(data), remain);
        if (!m_channel->isWriting())
        {
            m_channel->enableWriting();
        }
    }
}

void TcpConnection::shutdownInLoop()
{
    if (!m_channel->isWriting())
    {
        m_socket->shutdownWrite();
    }
}

void TcpConnection::handRead()
{
    int err_num = 0;
    size_t n = m_inputBuffer.readFd(m_channel->fd(), &err_num);
    if (n > 0)
    {
        LOG_INFO("收到数据");
        m_MessageCallBack(shared_from_this(), &m_inputBuffer);
    }
    else if (n == 0)
    {
        LOG_INFO("对端关闭连接");
        handleClose();
    }
    else
    {
        LOG_ERROR("读错误{}", err_num);
    }
}

void TcpConnection::handleClose()
{
    m_state = StateE::kDisconnected;
    m_channel->disableAll();
    TcpConnectionPtr guardThis(shared_from_this());
    m_CloseCallBack(guardThis);
}

void TcpConnection::handleError()
{
}
