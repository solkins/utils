#ifndef SVR_EPOLL_H
#define SVR_EPOLL_H

#include <atomic>
#include <functional>

class svr_epoll
{
public:
    svr_epoll();
    ~svr_epoll();

    void setport(unsigned short port)
    {
        m_port = port;
    }

    void start();
    void stop();
    void bind_server_cb(std::function<int(int, const char*, int, char*, int)> f)
    {
        server_cb = f;
    }

private:
    void epollproc();
    void handlerequest(int fd);
    void onnewconnection();

private:
    std::atomic<bool> running;
    int m_efd;
    int m_sock;
    unsigned short m_port;
    std::function<int(int, const char*, int, char*, int)> server_cb;
};

#endif // SVR_EPOLL_H
