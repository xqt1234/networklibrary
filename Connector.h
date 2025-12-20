#pragma once
#include "InetAddress.h"
#include "Channel.h"
#include <functional>
namespace mymuduo
{
    class Connector
    {
    public:
        using NewConnectionCallback = std::function<void(int)>;
    private:
        InetAddress m_serverAddr;
        enum class States
        {
            kDisconnected,
            kConnecting,
            kConnected
        };
        States m_state;
        NewConnectionCallback m_NewConnectionCallback;
        std::unique_ptr<Channel> m_channel;
        EventLoop *m_loop;
        EventCallBack m_retryCallBack;
    public:
        Connector(EventLoop *loop, const InetAddress &serverAddr);
        ~Connector();
        void connect();
        void setNewConnectionCallback(const NewConnectionCallback &cb)
        {
            m_NewConnectionCallback = cb;
        }
        void setRetryCallBack(const EventCallBack& cb)
        {
            m_retryCallBack = cb;
        }
    private:
        void connecting(int sockfd);
        void handleWrite();
        void handleError();
        void resetChannel();
        int removeAndResetChannel();
        
    };
}