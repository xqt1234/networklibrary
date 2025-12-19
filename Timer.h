#pragma once
#include "CallBacks.h"
#include <atomic>
#include "TimeStamp.h"
namespace mymuduo
{
    class Timer
    {
    private:
        const TimerCallBack m_cb;
        const double m_interval;
        TimeStamp m_expireTime;
        bool m_repeat;
        static std::atomic<int64_t> m_createSeq;
        const int64_t m_seq;
    public:
        Timer(TimerCallBack cb, TimeStamp expireTime, double interval);
        ~Timer() = default;
        void run()
        {
            m_cb();
        }
        bool repeat()
        {
            return m_repeat;
        }
        void restart(TimeStamp now);
        int64_t seq(){return m_seq;}
        const TimeStamp& expireTime() const{return m_expireTime;}
    };
}