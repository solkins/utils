#ifndef SOCKET_BASE_H
#define SOCKET_BASE_H

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#endif

#ifdef WIN32
typedef int socklen_t;
#else
#define INVALID_SOCKET      (~0)
#define SOCKET_ERROR		(-1)
#define closesocket         close
#define GetLastError()      errno
#endif

class socket
{
public:
    // SOCK_STREAM or SOCK_DGRAM
    socket(int type);
    ~socket();

    static socket attach(int sock);
    static unsigned long address2ulong(const char* address);

    // socket
    int getsockopt(int level, int optname, void *optval, socklen_t *optlen);
    int setsockopt(int level, int optname, const void *optval, socklen_t optlen);
    bool bind(unsigned short port, unsigned long address = INADDR_ANY);
    void close();
    bool canread(int ms);
    bool canwrite(int ms);

    // tcp
    bool listen(int n = 5);
    int accept(sockaddr_in* addr);
    bool connect(unsigned short port, unsigned long address);
    int send(const void* buf, size_t len);
    int recv(void* buf, size_t len);

    // udp
    bool joingroup(const char* groupip);
    int sendto(const void* buf, size_t len, unsigned short port, unsigned long address);
    int recvfrom(void* buf, size_t len, sockaddr_in* addr);

private:
    int m_sock;
};

#endif // SOCKET_BASE_H
