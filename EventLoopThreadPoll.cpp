#include "EventLoopThreadPoll.h"

EventLoopThreadPoll::EventLoopThreadPoll(EventLoop* baseLoop)
    :m_baseLoop(baseLoop)
{
}

EventLoopThreadPoll::~EventLoopThreadPoll()
{
}

void EventLoopThreadPoll::threadnum(int num)
{
    m_threadnum = num;
}

void EventLoopThreadPoll::start()
{
    for(int i = 0;i < m_threadnum;++i)
    {
        m_threads.push_back(new EventLoopThread());
    }
}

EventLoop *EventLoopThreadPoll::getNextLoop()
{
    EventLoop* loop = nullptr;
    if(m_threads.size() > 0)
    {
        loop = m_threads[m_currentLoopNum]->getLoop();
        m_currentLoopNum++;
        if(m_currentLoopNum>=m_threadnum)
        {
            m_currentLoopNum = 0;
        }
    }else
    {
        return m_baseLoop;
    }
    return loop;
}
