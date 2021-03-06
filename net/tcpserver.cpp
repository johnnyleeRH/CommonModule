#include "tcpserver.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "Log.h"

//#define MAXEVENTS 4
#define MAXLINE   4096

TcpServer::TcpServer(uint16_t tcpPort, size_t epollSize)
        : m_listenPort(tcpPort),
          m_epollfd(-1),
          m_listenfd(-1),
          m_events(NULL),
          m_running(true),
          m_pollsize(epollSize) {
    //one for listen, others for communicate
    m_events = (struct epoll_event *)calloc(m_pollsize + 1, sizeof(struct epoll_event));
}

TcpServer::~TcpServer() {
    INFO("destruct TcpServer.");
    if (-1 != m_epollfd)
        close(m_epollfd);
    if (-1 != m_listenfd)
        close(m_listenfd);
    for (int index = 0; index < m_pollsize; ++index)
        close(m_events[index].data.fd);
    free(m_events);
}

int TcpServer::CreateAndBind() {
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == m_listenfd) {
        ERROR("create server socket failed error %d.", errno);
        return -1;
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(m_listenPort);
    if (-1 == bind(m_listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) {
        ERROR("server socket bind error %d.", errno);
        return -1;
    }
    return 0;
}

int TcpServer::SetNonBlock(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (-1 == flags) {
        ERROR("get socket flags failed %d.", errno);
        return -1;
    }
    flags |= O_NONBLOCK;
    if (-1 == fcntl(sockfd, F_SETFL, flags)) {
        ERROR("set socket flags failed %d.", errno);
        return -1;
    }
    return 0;
}


void TcpServer::RecvTcpData(int readfd) {
    static char buf[MAXLINE];
    while(m_running) {
        ssize_t  count;
        count = read(readfd, buf, sizeof(buf));
        if (-1 == count) {
            if (errno != EAGAIN) {
                //EAGAIN means all data been read, go back.
                ERROR("read failed.");
            }
            break;
        } else if (count == 0) {
            break;
        } else {
            //handle the user data
        }
    }
}

void TcpServer::AcceptHandle() {
    struct sockaddr in_addr;
    socklen_t in_len = sizeof(in_addr);

    while (m_running) {
        int infd = accept(m_listenfd, &in_addr, &in_len);
        if (-1 == infd) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                return;
            else {
                ERROR("accept failed.");
                return;
            }
        }
        if (-1 == SetNonBlock(infd))
            return;
        struct epoll_event event;
        event.data.fd = infd;
        event.events = EPOLLIN | EPOLLET;
        if (-1 == epoll_ctl(m_epollfd, EPOLL_CTL_ADD, infd, &event)) {
            ERROR("epoll control add error %d.", errno);
            return;
        }
        INFO("new socket %d accepted.", infd);
    }
}

void TcpServer::HandleSockErr(const int fd) {
    if (-1 == epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL))
        ERROR("epoll control delete socket [%d] error %d.", fd, errno);
    else
        INFO("delete socket [%d] event success.", fd);
    close(fd);
}

void TcpServer::Stop() {
    m_running = false;
    m_iohandle.join();
}

void TcpServer::IOStart() {

    struct epoll_event event;
    event.data.fd = m_listenfd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (-1 == epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &event)) {
        ERROR("epoll control listen error %d.", errno);
        return;
    }

    if (-1 == listen(m_listenfd, SOMAXCONN)) {
        ERROR("server socket listen error %d.", errno);
        return;
    }

    // set wait time 1 sec
    const int kexpire = 1000;
    while (m_running) {
        int readynum = epoll_wait(m_epollfd, m_events, m_pollsize, kexpire);
        if (-1 == readynum) {
            ERROR("epoll wait error %d.", errno);
            continue;
        } else if (0 == readynum) {
            // timer expire
            continue;
        }
//        else {
//            INFO("ready number is %d.", readynum);
//        }
        for (int index = 0; index < readynum; ++index) {
//            INFO("index %d fd %d.", index, m_events[index].data.fd);
            if ((m_events[index].events & EPOLLERR) ||
                (m_events[index].events & EPOLLHUP) ||
                (m_events[index].events & EPOLLRDHUP) ||
                (!(m_events[index].events & EPOLLIN))) {
                ERROR("epoll error happen, events %u.", m_events[index].events);
                //close(m_events[index].data.fd);
                HandleSockErr(m_events[index].data.fd);
                continue;
            } else if (m_listenfd == m_events[index].data.fd) {
                INFO("listen index %d, fd %d.", index, m_listenfd);
                AcceptHandle();
                continue;
            } else {
                RecvTcpData(m_events[index].data.fd);
            }
        }
    }
}

int TcpServer::Start() {

    if (-1 == CreateAndBind())
        return -1;

    if (-1 == SetNonBlock(m_listenfd))
        return -1;

    if (-1 == (m_epollfd = epoll_create1(0))) {
        ERROR("epoll create error %d.", errno);
        return -1;
    }
    m_iohandle = boost::thread(&TcpServer::IOStart, this);
    INFO("tcp server success start.");
    return 0;

}
