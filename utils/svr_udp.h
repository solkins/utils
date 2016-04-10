#ifndef SVR_UDP_H
#define SVR_UDP_H

#include <functional>

class svr_udp
{
public:
    svr_udp();
    ~svr_udp();

    void setport(unsigned short port);
    void bind_server_cb(std::function<int(int, const char*, int, char*, int)> f);

    void start();
    void stop();

private:
    class svr_udp_imp* _imp;
};

#endif // SVR_UDP_H
