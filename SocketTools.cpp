#include "SocketTools.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
bool isSelfConnect(int sockfd)
{
    struct sockaddr_in local, peer;
    socklen_t len = sizeof(local);

    if (::getsockname(sockfd, (struct sockaddr*)&local, &len) < 0)
        return false;

    len = sizeof(peer);
    if (::getpeername(sockfd, (struct sockaddr*)&peer, &len) < 0)
        return false;

    return local.sin_port == peer.sin_port &&
           local.sin_addr.s_addr == peer.sin_addr.s_addr;
}

int getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    if(::getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&optval,&optlen) < 0)
    {
        return errno;
    }else
    {
        return optval;
    }
}
