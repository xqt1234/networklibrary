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
        TimerId(Timer* timer, int64_t seq)
            : m_timer(timer), m_seq(seq)
        {
        }
        ~TimerId() = default;
        friend class TimerQueue;
    };
}