#include "svr_udp.h"
#include <thread>
#include <iostream>
#include <string.h>

int svrproc(int fd, const char* req, int reqlen, char* resp, int respbuflen)
{
    ((char*)req)[reqlen] = 0;
    std::cout<<reqlen<<":"<<req<<std::endl;
    return 0;
}

int main(int argc, char* argv[])
{
    svr_udp s;
    s.setport(10000);
    s.bind_server_cb(svrproc);
    s.start();
    std::cout<<"server online"<<std::endl;
    while (1)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
