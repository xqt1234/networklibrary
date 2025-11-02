#pragma once
#include "CallBacks.h"
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
public:
    Channel(int fd);
    ~Channel();
    void set_revents(int revt) { m_revents = revt; }
    int fd() { return m_fd; }
    int events() const { return m_events; }
    void handleEvent();
    void handleEventWithGuard();
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
    void tie(const std::shared_ptr<void>& obj);
};
