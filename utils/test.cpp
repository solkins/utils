#include "svr_epoll.h"
#include <memory.h>

int svrproc(int fd, const char* req, int reqlen, char* resp, int resplen)
{
    memcpy(resp, req, reqlen);
    return reqlen;
}

int main(int argc, char* argv[])
{
    svr_epoll svr;
    svr.setport(8888);
    svr.bind_server_cb(svrproc);
    svr.start();
    getchar();
    svr.stop();
    return 0;
}
