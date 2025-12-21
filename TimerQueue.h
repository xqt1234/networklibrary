#pragma once
#include "Timer.h"
#include <set>
#include "CallBacks.h"
#include "TimeStamp.h"
#include "Timer.h"
#include "Channel.h"

#include "TimerId.h"
#include <unordered_set>
#include <atomic>
namespace mymuduo
{
    class EventLoop;
    class TimerQueue
    {
    public:
        using Entry = std::pair<TimeStamp,Timer*>;
        using ActiveEntry = std::pair<Timer*,int64_t>;
    private:
        std::set<Entry> m_TimerSet;// 正常set，时间排序
        std::set<ActiveEntry> m_managerSet;
        std::set<ActiveEntry> m_canceledSet;
        std::set<ActiveEntry> m_deleteSet;
        int m_timerfd;
        Channel m_channel;
        EventLoop* m_loop;
        std::atomic<bool> m_handdingCallBacks{false};
    public:
        TimerQueue(EventLoop * loop);
        ~TimerQueue();
        TimerId addTimer(TimerCallBack cb,TimeStamp startTime,double interval);
        void handleRead();
        void cancelInLoop(TimerId timer);
    private:
        void addTimerInLoop(Timer* timer);
        bool insert(Timer* timer);
        std::vector<Entry> getExpiredTimer(TimeStamp now);
        void resetTimer(std::vector<TimerQueue::Entry>& expiredVec,TimeStamp now);
    };
    
}