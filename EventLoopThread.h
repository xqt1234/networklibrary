#pragma once
#include "EventLoop.h"
#include <thread>
class EventLoopThread
{
private:
    std::thread m_thread;
    EventLoop* m_loop{nullptr};
public:
    EventLoopThread(/* args */);
    ~EventLoopThread();
    EventLoop* getLoop();
private:
    void EventLoopFunc();

};

