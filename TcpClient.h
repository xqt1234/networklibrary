#pragma once
#include "EventLoop.h"
#include "InetAddress.h"
#include "Connector.h"
#include "CallBacks.h"
#include <mutex>
#include "TcpConnection.h"
#include <atomic>
#include <optional>
namespace mymuduo
{
    class TcpClient
    {
    private:
        EventLoop *m_loop;
        InetAddress m_addr;
        std::string m_name;
        std::atomic<bool> m_isconnect{false};
        std::unique_ptr<Connector> m_connector;
        ConnectionCallBack m_ConnectionCallBack;
        MessageCallBack m_MessageCallBack;
        TcpConnectionPtr m_connection;
        std::mutex m_conMtx;
        std::atomic<bool> m_retry{false};
        static const int kInitRetryMs;
        static const int kMaxRetryMs;
        int m_currentRetryMs;
        TimerId m_timerid;
    public:
        TcpClient(EventLoop *loop, const InetAddress &addr, std::string name);
        ~TcpClient();
        void connect(bool retry = false);
        void disconnect();
        TcpConnectionPtr connection();
        const std::string &name() const
        {
            return m_name;
        }
        bool isConnected(){return m_isconnect;}
        void handNewConnection(int sockfd);
        void setConnectionCallBack(const ConnectionCallBack &cb)
        {
            m_ConnectionCallBack = cb;
        }
        void setMessageCallBack(const MessageCallBack &cb)
        {
            m_MessageCallBack = cb;
        }
        void removeConnection(const TcpConnectionPtr &conn);
        void retryConnection();
    };
}
