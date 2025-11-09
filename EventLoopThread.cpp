#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
{
    m_thread = std::thread(std::bind(&EventLoopThread::EventLoopFunc, this));
}

EventLoopThread::~EventLoopThread()
{
    if (m_loop)
    {
        m_loop->quit();
    }
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void EventLoopThread::EventLoopFunc()
{
    EventLoop loop;
    m_loop = &loop;
    loop.loop();
}

EventLoop *EventLoopThread::getLoop()
{
    return m_loop;
}
