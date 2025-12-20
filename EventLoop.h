#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include "EPollPoller.h"
#include "Channel.h"
#include <mutex>
#include "TimerQueue.h"
/**
 * @brief 核心类，在这里循环阻塞检查epoll中是否有事件发生和执行传入的回调函数，
 * 等待新事件的发生，所有的读写，事件处理的源头。
 * 对外提供入任务队列接口，
 * 如果不是自己所在线程或者循环正在处理事件，那么就通过写入唤醒fd，使epoll监听产生事件，
 * 醒来或者处理完后下次直接去执行新事件或函数。
 *
 *
 */
namespace mymuduo
{
    class EventLoop
    {
    public:
        using ChannelList = std::vector<Channel *>;
        using Functor = std::function<void()>;
        std::thread::id m_threadid;

    private:
        std::atomic<bool> m_quit{false};
        std::unique_ptr<EPollPoller> m_poller;
        ChannelList m_activeList;
        bool m_eventHandling{false};
        std::vector<Functor> m_pendingFunctors;
        std::mutex m_pendingMtx;

        int m_wakeupFd;
        std::unique_ptr<Channel> m_wakeupChannel;
        std::atomic<bool> m_callingPendingFunctors{false};
        std::unique_ptr<TimerQueue> m_timerQueue;

    public:
        EventLoop();
        ~EventLoop();
        void loop();
        void queueInLoop(Functor func);
        void runInLoop(Functor func);
        void updateChannel(Channel *channel);
        void removeChannel(Channel *channel);
        void quit();
        bool isInLoopThread()
        {
            return m_threadid == std::this_thread::get_id();
        }
        TimerId runAfter(TimerCallBack cb,double seconds);
        TimerId runAt(TimerCallBack cb,TimeStamp tTime);
        TimerId runEvery(TimerCallBack cb,double interval);
        void cancelTimer(TimerId timerid);
    private:
        void wakeup();
        void handleRead();
        void doPendingFunctors();
    };
}