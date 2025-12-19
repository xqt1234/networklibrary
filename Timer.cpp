#include "Timer.h"
using namespace mymuduo;
std::atomic<int64_t> Timer::m_createSeq = 0;
Timer::Timer(TimerCallBack cb, TimeStamp expireTime, double interval)
    :m_cb(cb),m_expireTime(expireTime),m_interval(interval),m_repeat(interval > 0.0),m_seq(m_createSeq++)
{

}

void Timer::restart(TimeStamp now)
{
    if(m_repeat)
    {
        m_expireTime = AddTime(now,m_interval);
    }else
    {
        m_expireTime = TimeStamp();
    }
}
