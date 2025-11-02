#pragma once
#include <unordered_map>
#include <vector>
#include "Channel.h"
class EPollPoller
{
public:
    using ChannelList = std::vector<Channel*>;
    using EventList =std::vector<struct epoll_event>;
    using ChannelMap = std::unordered_map<int,Channel*>;
private:
    ChannelMap m_clients;
    int m_epollfd;
    EventList m_events;
public:
    EPollPoller(/* args */);
    ~EPollPoller();
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel) const;
    void fillActiveChannels(int numEvents,
                                     ChannelList* activeChannels);
};
