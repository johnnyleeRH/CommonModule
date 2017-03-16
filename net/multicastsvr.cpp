#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#include "multicastsvr.hpp"
#include "log.hpp"

UdpSvr::UdpSvr(std::string addr, unsigned short port) {
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == fd) {
        ERROR("create multicast socket failed error %d.", errno);
        return;
    }

    memset(&destaddr, 0, sizeof(destaddr));
    destaddr.sin_family = AF_INET;
    destaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    destaddr.sin_port = htons(port);
}

UdpSvr::~UdpSvr() {
    close(fd);
}

void UdpSvr::SendMsg(char *msg, int len) {
    ssize_t sentsize = sendto(fd, msg, len, 0, (struct sockaddr *)&destaddr, sizeof(destaddr));
    if (-1 == sentsize)
        ERROR("send msg failed %d.", errno);
    else if (sentsize != len)
        INFO("sent size %d not equal to need size %d.", sentsize, len);
}
