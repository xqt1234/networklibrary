#pragma once
#include <netinet/in.h>
#include <string>
using std::string;
class InetAddress
{
private:
    sockaddr_in m_addr;
public:
    InetAddress(uint16_t port = 10000,string ip = "0.0.0.0");
    ~InetAddress();
    string toIpPortString() const;
    const sockaddr_in* getSockAddr() const{return &m_addr;}
    void setAddr(const sockaddr_in addr){m_addr = addr;}
};