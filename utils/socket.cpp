#include "socket.h"
#include <cctype>
#include <memory.h>

#ifdef WIN32
class netenv
{
public:
    netenv()
    {
        WSAStartup(MAKEWORD(2, 2), &wsadata);
    }
    ~netenv()
    {
        WSACleanup();
    }

private:
    WSADATA wsadata;
};
static netenv g_env;
#endif

sys_sock sys_sock::attach(int sock)
{
    sys_sock s;
    s.m_sock = sock;
    return s;
}

unsigned long sys_sock::address2ulong(const char* address)
{
    if (isdigit(address[0]))
        return inet_addr(address);

    struct hostent *hp;
    if ((hp = gethostbyname(address)) == NULL)
        return INVALID_SOCKET;

    return ((in_addr*)hp->h_addr)->s_addr;
}

sys_sock::sys_sock(int type)
{
    m_sock = ::socket(AF_INET, type, 0);
}

sys_sock::~sys_sock()
{
    close();
}

int sys_sock::getsockopt(int level, int optname, void *optval, socklen_t *optlen)
{
    return ::getsockopt(m_sock, level, optname, (char*)optval, optlen);
}

int sys_sock::setsockopt(int level, int optname, const void *optval, socklen_t optlen)
{
    return ::setsockopt(m_sock, level, optname, (const char*)optval, optlen);
}

bool sys_sock::bind(unsigned short port, unsigned long address)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof (sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = address;

    socklen_t len = sizeof (sockaddr_in);
    return ::bind(m_sock, (const sockaddr*)&addr, len) != SOCKET_ERROR;
}

void sys_sock::close()
{
    if (m_sock != INVALID_SOCKET)
        ::closesocket(m_sock);
    m_sock = INVALID_SOCKET;
}

int sys_sock::detach()
{
    int sock = m_sock;
    m_sock = INVALID_SOCKET;
    return sock;
}

bool sys_sock::canread(int ms)
{
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = ms * 1000;
    fd_set read;
    FD_ZERO(&read);
    FD_SET(m_sock, &read);
    return ::select(m_sock + 1, &read, NULL, NULL, &timeout) > 0;
}

bool sys_sock::canwrite(int ms)
{
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = ms * 1000;
    fd_set write;
    FD_ZERO(&write);
    FD_SET(m_sock, &write);
    return ::select(m_sock + 1, NULL, &write, NULL, &timeout) != 0;
}

bool sys_sock::listen(int n)
{
    return ::listen(m_sock, n) != SOCKET_ERROR;
}

int sys_sock::accept(sockaddr_in* addr)
{
    socklen_t addrlen = sizeof (sockaddr_in);
    return ::accept(m_sock, (sockaddr*) & addr, &addrlen);
}

bool sys_sock::connect(unsigned short port, unsigned long address)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof (sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = address;

    socklen_t len = sizeof (sockaddr_in);
    return ::connect(m_sock, (const sockaddr*)&addr, len) != SOCKET_ERROR;
}

int sys_sock::send(const void* buf, size_t len)
{
    return ::send(m_sock, (const char*)buf, len, 0);
}

int sys_sock::recv(void* buf, size_t len)
{
    return ::recv(m_sock, (char*)buf, len, 0);
}

bool sys_sock::joingroup(const char* groupip)
{
    unsigned char ttl=255;
    if (setsockopt(IPPROTO_IP, 10/*IP_MULTICAST_TTL*/, &ttl, sizeof(ttl)) == SOCKET_ERROR)
        return false;

    struct ip_mreq {
        struct in_addr imn_multiaddr;
        struct in_addr imr_interface;
    } areq;
    areq.imn_multiaddr.s_addr = inet_addr(groupip);
    areq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(IPPROTO_IP, 12/*IP_ADD_MEMBERSHIP*/, &areq, sizeof(areq)) == SOCKET_ERROR)
        return false;

    return true;
}

int sys_sock::sendto(const void* buf, size_t len, unsigned short port, unsigned long address)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof (sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = address;

    return sendto(buf, len, &addr);
}

int sys_sock::sendto(const void *buf, size_t len, sockaddr_in *addr)
{
    socklen_t addrlen = sizeof (sockaddr_in);
    return ::sendto(m_sock, (const char*)buf, len, 0, (const sockaddr*) addr, addrlen);
}

int sys_sock::recvfrom(void* buf, size_t len, sockaddr_in* addr)
{
    socklen_t addrlen = sizeof (sockaddr_in);
    return ::recvfrom(m_sock, (char*)buf, len, 0, (sockaddr*) addr, &addrlen);
}
