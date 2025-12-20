#pragma once
#include "Timer.h"
#include "TimerQueue.h"
namespace mymuduo
{
    class TimerId
    {
    private:
        Timer* m_timer;
        int64_t m_seq;

    public:
        TimerId():m_timer(nullptr),m_seq(-1){}
        TimerId(Timer* timer, int64_t seq = -1)
            : m_timer(timer), m_seq(seq)
        {
        }
        bool hasSet(){return m_seq != -1;}
        ~TimerId() = default;
        friend class TimerQueue;
    };
}