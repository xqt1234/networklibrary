#include "TimerQueue.h"
#include <sys/timerfd.h>
#include "Logger.h"
using namespace mymuduo;


int createTimerFd()
{
    int timefd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    if(timefd < 0)
    {
        LOG_FATAL("创建timerfd失败");
    }
    return timefd;
}
void readTimeFd(int timefd)
{
    uint64_t one;
    size_t len = ::read(timefd, (void *)&one, sizeof(one));
    if (len != sizeof(one))
    {
        LOG_ERROR("唤醒定时器失败");
    }

}
// struct itimerspec
//   {
//     struct timespec it_interval;
//     struct timespec it_value;
//   };
struct timespec calTimeDistance(TimeStamp expiration)
{
    int64_t distance = expiration.microSinceEpoch() - TimeStamp::now().microSinceEpoch();
    if(distance < 100)
    {
        distance = 100;
    }
    struct timespec res;
    res.tv_sec = static_cast<time_t>(distance/TimeStamp::kMicroSecondsPerSecond);
    res.tv_nsec = static_cast<long>((distance%TimeStamp::kMicroSecondsPerSecond) * 1000);
    return res;
}

void updateLocInTimerFd(int timeFd,TimeStamp expiration)
{
    struct itimerspec newVal{0};
    newVal.it_value = calTimeDistance(expiration);
    int res = ::timerfd_settime(timeFd,0,&newVal,nullptr);
    if(res < 0)
    {
        LOG_ERROR("更新时间失败");
    }
}
TimerQueue::TimerQueue(EventLoop *loop)
    :m_loop(loop)
    ,m_timerfd(createTimerFd())
    ,m_channel(m_timerfd,loop)
{
    m_channel.setReadCallBack(std::bind(&TimerQueue::handleRead,this));
    m_channel.enableReading();
}

TimerId TimerQueue::addTimer(TimerCallBack cb, TimeStamp startTime, double interval)
{
    Timer* timer = new Timer(cb,startTime,interval);
    m_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop,this,timer));
    return TimerId(timer,timer->seq());
}

void TimerQueue::handleRead()
{
    std::cout << "被触发-----" << std::endl;
    readTimeFd(m_timerfd);
    TimeStamp now = TimeStamp::now();
    std::vector<Entry> res = getExpiredTimer(now);
    m_handdingCallBacks = true;
    m_canceledSet.clear();
    for(auto& val : res)
    {
        val.second->run();
    }
    m_handdingCallBacks = false;
    resetTimer(res,now);
}
void mymuduo::TimerQueue::resetTimer(std::vector<Entry>& expiredVec,TimeStamp now)
{
    for(auto& val : expiredVec)
    {
        ActiveEntry expiredEntry(val.second,val.second->seq());
        if(val.second->repeat() && m_canceledSet.find(expiredEntry) == m_canceledSet.end())
        {
            val.second->restart(now);
            insert(val.second);
        }else
        {
            delete val.second;
        }
    }
    if(!m_TimerSet.empty())
    {
        TimeStamp newTime = m_TimerSet.begin()->second->expireTime();
        updateLocInTimerFd(m_timerfd,newTime);
    }

}


void mymuduo::TimerQueue::cancelInLoop(TimerId timer)
{
    ActiveEntry tmp(timer.m_timer,timer.m_seq);
    auto it = m_managerSet.find(tmp);
    if(it != m_managerSet.end())
    {
        m_TimerSet.erase(Entry(it->first->expireTime(),it->first));
        m_managerSet.erase(it);
    }
    // 这里，是在handleRead的val.second->run();中被调用，所以，此时m_expireSet已经clear了
    // 然后在后面清除并查找重用的时候，会查找到。
    if(m_handdingCallBacks)
    {
        m_canceledSet.insert(tmp);
    }
}

void mymuduo::TimerQueue::addTimerInLoop(Timer* timer)
{
    bool isFront = insert(timer);
    if(isFront)
    {
        updateLocInTimerFd(m_timerfd,timer->expireTime());
    }
}

bool mymuduo::TimerQueue::insert(Timer* timer)
{
    auto it = m_TimerSet.begin();
    const TimeStamp& expireTime = timer->expireTime();
    bool isFirstLoc = false;
    if(it == m_TimerSet.end() || expireTime < it->first)
    {
        isFirstLoc = true;
    }
    m_TimerSet.insert({expireTime,timer});
    m_managerSet.insert({timer,timer->seq()});
    return isFirstLoc;
}

std::vector<TimerQueue::Entry> TimerQueue::getExpiredTimer(TimeStamp now)
{
    Entry endEntry = {now,reinterpret_cast<Timer*>(UINTPTR_MAX)};
    auto it = m_TimerSet.lower_bound(endEntry);
    std::vector<Entry> expired(m_TimerSet.begin(),it);
    m_TimerSet.erase(m_TimerSet.begin(),it);
    for(auto& val : expired)
    {
        std::pair<Timer*,int64_t> toRemove(val.second, val.second->seq());
        m_managerSet.erase(toRemove);
    }
    return expired;
}


