#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
{
    m_thread = std::thread(std::bind(&EventLoopThread::EventLoopFunc,this));
}

EventLoopThread::~EventLoopThread()
{

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
