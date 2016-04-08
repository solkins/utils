#include "svr_epoll.h"
#include <sys/resource.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "socket.h"
#include "threadpool.h"

#define MAXEPOLLSIZE    10000

svr_epoll::svr_epoll()
{
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    setrlimit(RLIMIT_NOFILE, &rt);

    m_efd = epoll_create(MAXEPOLLSIZE);
    m_sock = INVALID_SOCKET;
    m_port = 0;
}

svr_epoll::~svr_epoll()
{
    stop();
    close(m_efd);
}

void svr_epoll::start()
{
    int flag = 1;
    sys_sock s(SOCK_STREAM);
    s.setsockopt(SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));

    if (!s.bind(m_port) || !s.listen())
        return;

    m_sock = s.detach();
    int flags = fcntl(m_sock, F_GETFL, 0);
    fcntl(m_sock, F_SETFL, flags | O_NONBLOCK);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = m_sock;
    epoll_ctl(m_efd, EPOLL_CTL_ADD, m_sock, &ev);

    std::thread(std::bind(&svr_epoll::epollproc, this)).detach();
}

void svr_epoll::stop()
{
    running = false;
}

void svr_epoll::epollproc()
{
    struct epoll_event events[MAXEPOLLSIZE];
    while (running)
    {
        int iCnt = epoll_wait(m_efd, events, MAXEPOLLSIZE, -1);
        for(int i = 0; i < iCnt; ++i)
        {
            int fd = events[i].data.fd;
            threadpoolrun(&svr_epoll::handlerequest, this, fd);
        }
    }
}

void svr_epoll::handlerequest(int fd)
{
    if (fd == m_sock)
    {
        onnewconnection();
        return;
    }

    sys_sock s = sys_sock::attach(fd);
    char buf[8192];
    int len = s.recv(buf, 8192);
    if (len <= 0)
    {
        struct epoll_event ev;
        ev.data.fd = fd;
        epoll_ctl(fd, EPOLL_CTL_DEL, fd,&ev);

        close(fd);
    }

    int resplen = 0;
    if (server_cb)
        resplen = server_cb(fd, buf, len, buf, 8192);
    if (resplen > 0)
        s.send(buf, resplen);
    s.detach();
}

void svr_epoll::onnewconnection()
{
    sys_sock s = sys_sock::attach(m_sock);
    while (s.canread())
    {
        sockaddr_in addr;
        int fdc = s.accept(&addr);

        if (fdc != SOCKET_ERROR)
        {
            int flags = fcntl(fdc, F_GETFL, 0);
            fcntl(fdc, F_SETFL, flags | O_NONBLOCK);

            struct epoll_event ev;
            ev.data.fd = fdc;
            ev.events = EPOLLIN | EPOLLET;
            epoll_ctl(m_efd, EPOLL_CTL_ADD, fdc, &ev);
        }
    }
    s.detach();
}
