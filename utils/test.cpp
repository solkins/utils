#include "svr_udp.h"
#include <thread>
#include <iostream>
#include <string.h>
#include "socket.h"

int svrproc(int fd, const char* req, int reqlen, char* resp, int respbuflen, unsigned long ip, unsigned short port)
{
    ((char*)req)[reqlen] = 0;
    std::cout<<inet_ntoa(*(in_addr*)&ip)<<":"<<port<<"->"<<reqlen<< " " <<req<<std::endl;
    strcpy(resp, req);
    return reqlen;
}

int main(int argc, char* argv[])
{
    svr_udp s;
    s.setport(10000);
    s.bind_server_cb(svrproc);
    s.start();
    s.stop();
    s.start();
    s.stop();
    s.start();
    s.stop();
    s.start();
    s.stop();
    s.start();
    s.stop();
    s.start();
    std::cout<<"server online"<<std::endl;
    while (1)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
