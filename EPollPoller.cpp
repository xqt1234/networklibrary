#include "EPollPoller.h"
#include <sys/epoll.h>
#include <errno.h>
#include "Logger.h"
EPollPoller::EPollPoller()
    : m_epollfd(::epoll_create1(EPOLL_CLOEXEC))
{
}

EPollPoller::~EPollPoller()
{
}

void EPollPoller::removeChannel(Channel *channel)
{
    auto it = m_clients.find(channel->fd());
    if (it != m_clients.end())
    {
        m_clients.erase(it);
    }
}

bool EPollPoller::hasChannel(Channel *channel) const
{
    auto it = m_clients.find(channel->fd());
    return it != m_clients.end();
}


void EPollPoller::fillActiveChannels(int timeout, ChannelList *activeChannels)
{
    int numEvents = ::epoll_wait(m_epollfd, &(*m_events.begin()), static_cast<int>(m_events.size()), timeout);
    int savedErrno = errno;
    if (numEvents > 0)
    {
        for (int i = 0; i < numEvents; ++i)
        {
            int fd = m_events[i].data.fd;
            if (m_clients.find(fd) != m_clients.end())
            {
                m_clients[fd]->set_revents(m_events[i].events);
                activeChannels->push_back(m_clients[fd]);
            }
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
