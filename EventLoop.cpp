#include "EventLoop.h"
#include <unistd.h>
#include "Logger.h"
const int kPollTimeMs = 10000;
EventLoop::EventLoop()
    : m_poller(new EPollPoller())
    , m_threadid(std::this_thread::get_id())
{
}

EventLoop::~EventLoop()
{
    m_quit = true;
}

void EventLoop::loop()
{
    LOG_INFO("开始循环loop");
    while (!m_quit)
    {
        m_activeList.clear();
        m_poller->fillActiveChannels(kPollTimeMs, &m_activeList);
        m_eventHandling = true;
        for (Channel *channel : m_activeList)
        {
            channel->handleEvent();
        }
    }
}

void EventLoop::queueInLoop(Functor func)
{
    {
        std::mutex m_pendingMtx;
        m_pendingFunctors.push_back(func);
    }
    if(!isInLoopThread() || m_callingPendingFunctors)
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor func)
{
    if(isInLoopThread())
    {
        func();
    }else
    {
        queueInLoop(func);
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    size_t len = write(m_wakeupFd,(void*)&one,sizeof(one));
    if(len != sizeof(one))
    {
        LOG_ERROR("唤醒失败");
    }
}

void EventLoop::doPendingFunctors()
{
    m_callingPendingFunctors = true;
    std::vector<Functor> funcs;
    {
        std::unique_lock<std::mutex> lock(m_pendingMtx);
        funcs.swap(m_pendingFunctors);
    }
    for(const auto& func : funcs)
    {
        func();
    }
    m_callingPendingFunctors = false;
}
