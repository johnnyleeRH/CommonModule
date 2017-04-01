#ifndef SSEMD_MULTICASTSVR_HPP
#define SSEMD_MULTICASTSVR_HPP

#include <netinet/in.h>
#include <string>

class UdpSvr {
public:
    explicit UdpSvr(std::string localip, std::string addr, unsigned short port);
    virtual ~UdpSvr();
    void SendMsg(char *msg, int len);
private:
    int fd;
    struct sockaddr_in destaddr;
};

#endif //SSEMD_MULTICASTSVR_HPP
