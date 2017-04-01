#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#include "multicastclient.hpp"
#include "log.hpp"

UdpClient::UdpClient(std::string localip, std::string addr, unsigned short port) {
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("create multicast socket failed error %d.", errno);
        return;
    }

    int sockbufsize = 65535;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*) &sockbufsize, sizeof(sockbufsize));
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse,
                   sizeof(reuse)) < 0) {
        ERROR("set udp reuseable failed %d.", errno);
        return;
    }

    memset(&srcaddr, 0, sizeof(srcaddr));
    srcaddr.sin_family = AF_INET;
    srcaddr.sin_addr.s_addr = INADDR_ANY;
    srcaddr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&srcaddr, sizeof(srcaddr)) < 0) {
        ERROR("bind failed %d.", errno);
        return;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(addr.c_str());
    mreq.imr_interface.s_addr = inet_addr(localip.c_str());
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        ERROR("set memship failed %d.", errno);
        return;
    }
}

UdpClient::~UdpClient() {
    close(fd);
}

ssize_t UdpClient::RecvMsg(char* buf, int bufsize) {
    socklen_t addrlen = sizeof(srcaddr);
    ssize_t bytes = recvfrom(fd, buf, bufsize, 0,
                             (struct sockaddr *)&srcaddr, &addrlen);
    if (-1 == bytes)
        ERROR("recv error %d happened.\n", errno);
    return  bytes;
}
