#ifndef SSEMD_MULTICASTCLIENT_HPP
#define SSEMD_MULTICASTCLIENT_HPP

#include <netinet/in.h>
#include <string>

class UdpClient {
public:
    explicit UdpClient(std::string localip, std::string addr, unsigned short port);
    virtual ~UdpClient();
    ssize_t RecvMsg(char*, int);

private:
    int fd;
    struct sockaddr_in srcaddr;
};

#endif //SSEMD_MULTICASTCLIENT_HPP
