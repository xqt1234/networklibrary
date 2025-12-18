#include "InetAddress.h"
#include <arpa/inet.h>
#include "Logger.h"
using namespace mymuduo;
InetAddress::InetAddress(uint16_t port, string ip)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(inet_pton(AF_INET,ip.c_str(),&addr.sin_addr) <= 0)
    {
        // addr.sin_addr.s_addr = INADDR_ANY;
        LOG_FATAL("ip转换失败");
    }
    m_addr = addr;

}

InetAddress::~InetAddress()
{
}

string InetAddress::toIpPortString() const
{
    char buf[INET_ADDRSTRLEN]{};
    const char* res= inet_ntop(AF_INET,&m_addr.sin_addr,buf,sizeof(buf));
    if(res != nullptr)
    {
        return std::format("{}:{}", res, 
                      ntohs(m_addr.sin_port));
    }
    LOG_ERROR("ip转换错误");
    return string();
    
}
