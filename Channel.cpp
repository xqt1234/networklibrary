#include "Channel.h"
#include "Logger.h"
#include <sys/epoll.h>
#include "EventLoop.h"
using namespace mymuduo;
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;
Channel::Channel(int fd,EventLoop* loop)
    : m_fd(fd)
    , m_loop(loop)
{
}

Channel::~Channel()
{
    LOG_DEBUG("析构了Channel channelfd: {}",m_fd);
}

void Channel::handleEvent()
{
    std::shared_ptr<void> guard;
    // 一些动态添加的连接之类的对象会进行绑定，这里进行检测是否已经销毁
    // 一些不是动态添加的对象，直接执行就行了。
    if (m_tied)
    {
        guard = m_tie.lock();
        if (guard)
        {
            handleEventWithGuard();
        }
    }
    else
    {
        handleEventWithGuard();
    }
}

void Channel::handleEventWithGuard()
{
    LOG_INFO("处理事件:{}", m_revents);
    if ((m_revents & EPOLLHUP) && !(m_revents & EPOLLIN) || (m_revents & EPOLLRDHUP))
    {
        LOG_DEBUG("关闭连接aaaaa");
        if (m_closeCallBack)
        {
            m_closeCallBack();
        }
    }
    if (m_revents & EPOLLERR)
    {
        if (m_errorCallBack)
        {
            m_errorCallBack();
        }
    }
    if (m_revents & (EPOLLIN | EPOLLPRI))
    {
        if (m_readCallBack)
        {
            m_readCallBack();
        }
    }
    if (m_revents & EPOLLOUT)
    {
        if (m_writeCallBack)
        {
            m_writeCallBack();
        }
    }
}

void Channel::update()
{
    m_addedToLoop = true;
    m_loop->updateChannel(this);
}

void Channel::remove()
{
    m_addedToLoop = false;
    m_loop->removeChannel(this);
    // if(!m_loop->isInLoopThread())
    // {
    //     std::cout << "不在自己的loop中" << std::endl;
    // }
    
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    m_tie = obj;
    m_tied = true;
}
