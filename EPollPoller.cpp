#include "EPollPoller.h"
#include <sys/epoll.h>
#include <errno.h>
#include "Logger.h"

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
EPollPoller::EPollPoller()
    : m_epollfd(::epoll_create1(EPOLL_CLOEXEC)), m_events(kInitEventListSize)
{
}

EPollPoller::~EPollPoller()
{
}

void EPollPoller::removeChannel(Channel *channel)
{
    // auto it = m_clients.find(channel->fd());
    // int state = channel->getState();
    // if (it != m_clients.end())
    // {
    //     m_clients.erase(it);
    // }
}

bool EPollPoller::hasChannel(Channel *channel) const
{
    // auto it = m_clients.find(channel->fd());
    // return it != m_clients.end();
    return true;
}

void EPollPoller::fillActiveChannels(int timeout, ChannelList *activeChannels)
{
    int numEvents = ::epoll_wait(m_epollfd, &(*m_events.begin()), static_cast<int>(m_events.size()), timeout);
    int savedErrno = errno;
    if (numEvents > 0)
    {
        for (int i = 0; i < numEvents; ++i)
        {
            Channel *channel = static_cast<Channel *>(m_events[i].data.ptr);
            activeChannels->push_back(channel);
            channel->set_revents(m_events[i].events);
        }
    }
    else if (numEvents == 0)
    {
        LOG_INFO("epoll没有收到消息");
    }
    else
    {
        if (savedErrno != EINTR)
        {
            LOG_ERROR("epoll发生错误");
        }
    }
}

void EPollPoller::updateChannel(Channel *channel)
{
    int state = channel->getState();
    if (state == kNew || state == kDeleted)
    {
        // if (state == kNew)
        // {
        //     int fd = channel->fd();
        //     m_clients[fd] = channel;
        // }
        channel->setState(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setState(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::update(int option, Channel *channel)
{
    struct epoll_event event{};
    int fd = channel->fd();
    event.data.ptr = channel;
    event.events = channel->events();
    if (::epoll_ctl(m_epollfd, option, fd, &event) < 0)
    {
        if (option == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll ctl删除连接错误");
        }
        else
        {
            LOG_ERROR("epoll ctl 其他错误");
        }
    }
}
