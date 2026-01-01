#include "protoBuilder.h"
#include <vector>
#include <endian.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
using namespace mymuduo;
void mymuduo::ProtoBuilder::enCodeRequest(const std::string &request, std::function<void(const std::string &)> callback)
{
    int totallen = sizeof(MsgHeader) + request.length();
    std::vector<char> sendvec(totallen);
    MsgHeader* header = reinterpret_cast<MsgHeader*>(sendvec.data());
    header->datalength = htonl(request.length());
    header->magic = htonl(kMagicNumber);
    memcpy(sendvec.data() + sizeof(MsgHeader),request.data(),request.length());
    callback(std::string(sendvec.data(),sendvec.size()));
}

void mymuduo::ProtoBuilder::deCodeResponse(mymuduo::Buffer *buffer, std::function<void(const std::string &)> callback)
{
    const char *data = buffer->peek();
    int len = buffer->readableBytes();
    while (len > sizeof(MsgHeader))
    {
        MsgHeader rpchead;
        memcpy(&rpchead,buffer->peek(),sizeof(MsgHeader));
        uint32_t magic = ntohl(rpchead.magic);
        if (kMagicNumber != magic)
        {
            buffer->retrieve(1);
            len = buffer->readableBytes();
            data = buffer->peek();
            continue;
        }
        uint32_t datalen = ntohl(rpchead.datalength);
        if (len < sizeof(MsgHeader) + datalen)
        {
            break;
        }
        buffer->retrieve(sizeof(MsgHeader));
        std::string msg = buffer->readAsString(datalen);
        data = buffer->peek();
        len = buffer->readableBytes();
        callback(msg);
    }
}
