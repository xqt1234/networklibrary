#pragma once
#include "EventLoopThread.h"
#include <vector>
class EventLoopThreadPoll
{
private:
    /* data */
    std::vector<EventLoopThread*> m_threads;
    int m_threadnum{0};
    int m_currentLoopNum{0};
    EventLoop* m_baseLoop;
public:
    EventLoopThreadPoll(EventLoop* baseLoop);
    ~EventLoopThreadPoll();
    void threadnum(int num);
    void start();
    EventLoop* getNextLoop();
};

