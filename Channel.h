#pragma once
#include "CallBacks.h"
class EventLoop;
class Channel
{
private:
    int m_fd;
    int m_events;
    int m_revents;
    ReadEventCallBack m_readCallBack;
    EventCallBack m_writeCallBack;
    EventCallBack m_closeCallBack;
    EventCallBack m_errorCallBack;
    std::weak_ptr<void> m_tie;
    bool m_tied{false};
    int m_state{0}; // 状态
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    EventLoop* m_loop;
public:
    Channel(int fd);
    ~Channel();
    void set_revents(int revt) { m_revents = revt; }
    int fd() { return m_fd; }
    void setState(int state) { m_state = state; }
    int getState() { return m_state; }
    int events() const { return m_events; }
    void handleEvent();
    void handleEventWithGuard();
    bool isNoneEvent() { return m_state == kNoneEvent; }
    bool isReadEvent() { return m_state & kReadEvent; }
    bool isWriteEvent() { return m_state & kWriteEvent; }
    void enableReading(){m_state | kReadEvent;}
    void enableWriting(){m_state | kWriteEvent;}
    void update();
    void setReadCallBack(ReadEventCallBack &cb)
    {
        m_readCallBack = cb;
    }
    void setWriteCallBack(EventCallBack &cb)
    {
        m_writeCallBack = cb;
    }
    void setCloseCallBack(EventCallBack &cb)
    {
        m_closeCallBack = cb;
    }
    void setErrorCallBack(EventCallBack &cb)
    {
        m_errorCallBack = cb;
    }
    void tie(const std::shared_ptr<void> &obj);
};
