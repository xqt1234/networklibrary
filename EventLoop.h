#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include "EPollPoller.h"
#include "Channel.h"
#include <mutex>
class EventLoop
{
public:
    using ChannelList =std::vector<Channel*>;
    using Functor = std::function<void()>;
    std::thread::id m_threadid;
private:
    std::atomic<bool> m_quit{false};
    EPollPoller* m_poller;
    ChannelList m_activeList;
    bool m_eventHandling{false};
    std::vector<Functor> m_pendingFunctors;
    std::mutex m_pendingMtx;
    
    int m_wakeupFd;
    Channel* m_wakeupChannel;
    std::atomic<bool> m_callingPendingFunctors{false};
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void queueInLoop(Functor func);
    void runInLoop(Functor func);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool isInLoopThread()
    {
        return m_threadid == std::this_thread::get_id();
    }
private:
    
    void wakeup();
    void handleRead();
    void doPendingFunctors();
};