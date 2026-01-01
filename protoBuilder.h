#pragma once
#include <string>
#include <atomic>
#include <Buffer.h>
#include <functional>
#include <memory>
namespace mymuduo
{
    struct MsgHeader
    {
        uint32_t magic;
        uint32_t datalength;
    };
    static const uint32_t kMagicNumber = 0x55AA55AA; 
    class ProtoBuilder
    {
    private:
        /* data */
    public:
        ProtoBuilder(/* args */) = default;
        ~ProtoBuilder()= default;
        static void enCodeRequest(const std::string& request,std::function<void(const std::string&)> callback);
        static void deCodeResponse(mymuduo::Buffer* buffer,std::function<void(const std::string&)> callback);
    };
}
