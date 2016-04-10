#ifndef SVR_IOCP_H
#define SVR_IOCP_H

#include <functional>

class svr_iocp
{
public:
    svr_iocp();
    ~svr_iocp();

    void setport(unsigned short port);
    void bind_server_cb(std::function<int(int, const char*, int, char*, int)> f);
    void start();
    void stop();

private:
    class svr_iocp_imp* _imp;
};

#endif // SVR_IOCP_H
