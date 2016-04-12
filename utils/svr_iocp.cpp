#include "svr_iocp.h"
#include <atomic>
#include <thread>
#include <mswsock.h>
#include "socket.h"
#include "sysinfo.h"

#define SEND 0
#define RECV 1
#define ACCEPT 2

#define DATA_LENGTH 8192

typedef struct{
    OVERLAPPED overlapped;
    WSABUF buffer;
    char databuf[DATA_LENGTH];
    int buflen;
    int operation;
    SOCKET client;
    sockaddr_in clientAddr;
    void* app;
}PER_IO_DATA,*LPPER_IO_DATA;

class svr_iocp_imp
{
public:
    svr_iocp_imp();
    ~svr_iocp_imp();

    void setport(unsigned short port)
    {
        m_port = port;
    }

    void bind_server_cb(std::function<int(int, const char*, int, char*, int, unsigned long, unsigned short)> f)
    {
        server_cb = f;
    }

    void start();
    void stop();

private:
    std::atomic<bool> running;
    void* m_hiocp;
    int m_sock;
    unsigned short m_port;
    std::function<int(int, const char*, int, char*, int, unsigned long, unsigned short)> server_cb;

    friend void AcceptConnection(svr_iocp_imp* _this);
    friend void HandleListen(unsigned short port, void* app);
    friend void HandleAccept(HANDLE CompletionPort, LPPER_IO_DATA perIoData);
    friend void ServerThread(HANDLE CompletionPort, void* app);
    friend void HandleRecv(DWORD bytes, LPPER_IO_DATA perIoData);
};

LPFN_ACCEPTEX getAcceptEXFun(int sock)
{
    LPFN_ACCEPTEX lpfnAcceptEx = nullptr;
    GUID guidAcceptEx = WSAID_ACCEPTEX;
    DWORD dwBytes = 0;
    WSAIoctl(sock,SIO_GET_EXTENSION_FUNCTION_POINTER,
             &guidAcceptEx,sizeof(guidAcceptEx),
             &lpfnAcceptEx,sizeof(lpfnAcceptEx),
             &dwBytes,NULL,NULL);
    return lpfnAcceptEx;
}

LPFN_GETACCEPTEXSOCKADDRS getGetAcceptExSockAddrsFun(int sock)
{
    LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockAddrs = nullptr;
    GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    DWORD dwBytes = 0;
    WSAIoctl(sock,SIO_GET_EXTENSION_FUNCTION_POINTER,
             &guidGetAcceptExSockAddrs,sizeof(guidGetAcceptExSockAddrs),
             &lpfnGetAcceptExSockAddrs,sizeof(lpfnGetAcceptExSockAddrs),
             &dwBytes,NULL,NULL);
    return lpfnGetAcceptExSockAddrs;
}

void AcceptConnection(svr_iocp_imp* _this)
{
    static LPFN_ACCEPTEX lpfnAcceptEx = getAcceptEXFun(_this->m_sock);

    LPPER_IO_DATA perIoData = new PER_IO_DATA;
    memset(&(perIoData->overlapped),0,sizeof(OVERLAPPED));
    perIoData->buffer.buf = perIoData->databuf;
    perIoData->buffer.len = perIoData->buflen = DATA_LENGTH;
    perIoData->operation = ACCEPT;
    perIoData->client = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,0,0,WSA_FLAG_OVERLAPPED);
    perIoData->app = _this;

    lpfnAcceptEx(_this->m_sock, perIoData->client, perIoData->databuf,
                 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16,
                 NULL, &(perIoData->overlapped));
}

void HandleListen(unsigned short port, void* app)
{
    svr_iocp_imp* _this = (svr_iocp_imp*)app;
    int Listen = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
    sys_sock s = sys_sock::attach(Listen);

    if (!s.bind(port) || !s.listen())
        return;

    _this->m_sock = s.detach();

    CreateIoCompletionPort((HANDLE)_this->m_sock, _this->m_hiocp, 0, 0);
    AcceptConnection(_this);
}

void HandleAccept(HANDLE CompletionPort, LPPER_IO_DATA perIoData)
{
    svr_iocp_imp* _this = (svr_iocp_imp*)perIoData->app;
    static LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockAddrs = getGetAcceptExSockAddrsFun(_this->m_sock);

    sockaddr_in* remote = NULL;
    sockaddr_in* local = NULL;
    int remoteLen = sizeof(sockaddr_in);
    int localLen = sizeof(sockaddr_in);
    lpfnGetAcceptExSockAddrs(perIoData->buffer.buf, 0,
                             sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
                             (LPSOCKADDR*)&local, &localLen,
                             (LPSOCKADDR*)&remote, &remoteLen);

    memcpy(&perIoData->clientAddr, remote, remoteLen);
    setsockopt(perIoData->client, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
        (char*)&(_this->m_sock), sizeof(int));

    CreateIoCompletionPort((HANDLE)perIoData->client, CompletionPort, 0, 0);

    memset(&(perIoData->overlapped),0,sizeof(OVERLAPPED));
    perIoData->operation = RECV;

    DWORD Flags = 0;
    WSARecv(perIoData->client, &(perIoData->buffer), 1,
            NULL, &Flags, &(perIoData->overlapped), NULL);

    AcceptConnection(_this);
}

void HandleSend(LPPER_IO_DATA perIoData)
{
    DWORD Flags = 0;
    perIoData->operation = RECV;
    perIoData->buffer.len = DATA_LENGTH;
    ZeroMemory(&(perIoData->overlapped),sizeof(OVERLAPPED));
    WSARecv(perIoData->client, &(perIoData->buffer), 1,
        NULL, &Flags, &(perIoData->overlapped), NULL);
}

void HandleRecv(DWORD bytes, LPPER_IO_DATA perIoData)
{
    svr_iocp_imp* _this = (svr_iocp_imp*)perIoData->app;
    if (_this->server_cb)
        bytes = _this->server_cb(perIoData->client, perIoData->buffer.buf, bytes,
                                 perIoData->databuf, perIoData->buflen,
                                 perIoData->clientAddr.sin_addr.s_addr,
                                 ntohs(perIoData->clientAddr.sin_port));
    if (bytes > 0)
    {
        perIoData->operation = SEND;
        perIoData->buffer.len = bytes;
        ZeroMemory(&(perIoData->overlapped),sizeof(OVERLAPPED));
        WSASend(perIoData->client, &(perIoData->buffer), 1,
            NULL, 0, &(perIoData->overlapped), NULL);
    }
    else
        HandleSend(perIoData);
}

void HandleClose(LPPER_IO_DATA perIoData)
{
    closesocket(perIoData->client);
    delete perIoData;
}

void ServerThread(HANDLE CompletionPort, void* app)
{
    svr_iocp_imp* _this = (svr_iocp_imp*)app;
     DWORD bytes;
     ULONG_PTR ck = 0;
     LPPER_IO_DATA perIoData = NULL;
     while(_this->running)
     {
         bytes = -1;
         GetQueuedCompletionStatus(
             CompletionPort,
             &bytes,
             (PULONG_PTR)&ck,
             (LPOVERLAPPED*)&perIoData,
             INFINITE);

         if(bytes == 0 && (perIoData->operation == RECV ||
             perIoData->operation == SEND))
         {
             HandleClose(perIoData);
             continue;
         }
         if(perIoData->operation == ACCEPT)
         {
             HandleAccept(CompletionPort, perIoData);
             continue;
         }
         if(perIoData->operation == SEND)
         {
             HandleSend(perIoData);
             continue;
         }
         if(perIoData->operation == RECV)
         {
             HandleRecv(bytes, perIoData);
             continue;
         }
     }
}

svr_iocp_imp::svr_iocp_imp()
{
    m_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
}

svr_iocp_imp::~svr_iocp_imp()
{
    stop();
    CloseHandle(m_hiocp);
}

void svr_iocp_imp::start()
{
    HandleListen(m_port, this);

    if (m_sock != INVALID_SOCKET)
    {
        running = true;
        for(int i=0; i<cpu_count(); i++)
            std::thread(ServerThread,m_hiocp, this).detach();
    }
}

void svr_iocp_imp::stop()
{
    running = false;
    closesocket(m_sock);
}

svr_iocp::svr_iocp()
{
    _imp = new svr_iocp_imp();
}

svr_iocp::~svr_iocp()
{
    delete _imp;
}

void svr_iocp::setport(unsigned short port)
{
    _imp->setport(port);
}

void svr_iocp::bind_server_cb(std::function<int(int, const char*, int, char*, int, unsigned long, unsigned short)> f)
{
    _imp->bind_server_cb(f);
}

void svr_iocp::start()
{
    _imp->start();
}

void svr_iocp::stop()
{
    _imp->stop();
}
