#ifndef SVR_EPOLL_H
#define SVR_EPOLL_H

#include <functional>

class svr_epoll
{
public:
    svr_epoll();
    ~svr_epoll();

    void setport(unsigned short port);
    void bind_server_cb(std::function<int(int, const char*, int, char*, int, unsigned long, unsigned short)> f);

    void start();
    void stop();

private:
	class svr_epoll_imp* _imp;
};

#endif // SVR_EPOLL_H
