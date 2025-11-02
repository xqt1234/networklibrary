#pragma once
#include <functional>
#include <memory>
class TcpConnection;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using EventCallBack = std::function<void()>;
using ConnectionCallBack = std::function<void(const TcpConnectionPtr&)>;
using CloseCallBack = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallBack = std::function<void(const TcpConnectionPtr&)>;
using ReadEventCallBack = std::function<void()>;

