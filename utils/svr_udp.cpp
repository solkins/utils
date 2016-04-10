#include "svr_udp.h"
#include "socket.h"
#include <atomic>
#include <thread>
#include "threadpool.h"
#include "pool.h"

class svr_udp_imp
{
public:
    svr_udp_imp();
    ~svr_udp_imp();

    void setport(unsigned short port)
    {
        m_port = port;
    }

    void bind_server_cb(std::function<int(int, const char*, int, char*, int)> f)
    {
        server_cb = f;
    }

    void start();
    void stop();

private:
    std::atomic<bool> running;
    int m_sock;
    unsigned short m_port;
    std::function<int(int, const char*, int, char*, int)> server_cb;

    friend void serverproc(void* app);
    friend void HandleRequest(svr_udp_imp* svr);
};

#define DATA_LENGTH 8192

struct SOCKBUF
{
    char buf[DATA_LENGTH];
};

void HandleRequest(svr_udp_imp* svr)
{
    sys_sock s = sys_sock::attach(svr->m_sock);
    int recvlen, sendlen;
    sockaddr_in addr;
    auto item = pool<SOCKBUF>::instance().get();
    recvlen = s.recvfrom(item->buf, DATA_LENGTH, &addr);
    if (recvlen > 0)
    {
        if (svr->server_cb)
        {
            sendlen = svr->server_cb(svr->m_sock, item->buf, recvlen, item->buf, DATA_LENGTH);
            if (sendlen > 0)
                s.sendto(item->buf, sendlen, &addr);
        }
    }
    s.detach();
}

void serverproc(void* app)
{
    svr_udp_imp* _this = (svr_udp_imp*)app;
    sys_sock s = sys_sock::attach(_this->m_sock);
    while (_this->running)
    {
        if (s.canread())
            threadpoolrun(HandleRequest, _this);
    }
    s.detach();
}

svr_udp_imp::svr_udp_imp()
{
    pool<SOCKBUF>::instance().init(cpu_count()*2);
}

svr_udp_imp::~svr_udp_imp()
{
    stop();
}

void svr_udp_imp::start()
{
    sys_sock s(SOCK_DGRAM);
    if (s.bind(m_port))
    {
        m_sock = s.detach();
        running = true;

        std::thread(serverproc, this).detach();
    }
}

void svr_udp_imp::stop()
{
    running = false;
    closesocket(m_sock);
}

svr_udp::svr_udp()
{
    _imp = new svr_udp_imp();
}

svr_udp::~svr_udp()
{
    delete _imp;
}

void svr_udp::setport(unsigned short port)
{
    _imp->setport(port);
}

void svr_udp::bind_server_cb(std::function<int(int, const char*, int, char*, int)> f)
{
    _imp->bind_server_cb(f);
}

void svr_udp::start()
{
    _imp->start();
}

void svr_udp::stop()
{
    _imp->stop();
}
