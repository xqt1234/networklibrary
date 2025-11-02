#include "Channel.h"
#include "Logger.h"
#include <sys/epoll.h>
Channel::Channel(int fd)
    :m_fd(fd)
{

}

void Channel::handleEvent()
{
    std::shared_ptr<void> guard;
    //一些动态添加的连接之类的对象会进行绑定，这里进行检测是否已经销毁
    //一些不是动态添加的对象，直接执行就行了。
    if(m_tied)
    {
        guard = m_tie.lock();
        if(guard)
        {
            handleEventWithGuard();
        }
    }else
    {
        handleEventWithGuard();
    }
}

void Channel::handleEventWithGuard()
{
    LOG_INFO("处理事件:{}",m_revents);
    if((m_revents & EPOLLHUP)&& !(m_revents & EPOLLIN))
    {
        if(m_closeCallBack)
        {
            m_closeCallBack();
        }
    }
    if(m_revents & EPOLLERR)
    {
        if(m_errorCallBack)
        {
            m_errorCallBack();
        }
    }
    if(m_revents & (EPOLLIN | EPOLLPRI))
    {
        if(m_readCallBack)
        {
            m_readCallBack();
        }
    }
    if(m_revents & EPOLLOUT)
    {
        if(m_writeCallBack)
        {
            m_writeCallBack();
        }
    }
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    m_tie = obj;
    m_tied = true;
}
