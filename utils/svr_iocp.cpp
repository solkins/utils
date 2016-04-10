#include "svr_iocp.h"
#include <atomic>
#include <thread>
#include <mswsock.h>
#include "socket.h"
#include "sysinfo.h"

#define SEND 0
#define RECV 1
#define ACCEPT 2

#define DATA_LENGTH 1000

typedef struct _PER_HANDLE_DATA
{
     SOCKET socket;
     SOCKADDR_STORAGE clientAddr;
}PER_HANDLE_DATA,*LPPER_HANDLE_DATA;

typedef struct{
     OVERLAPPED overlapped;
     WSABUF buffer;
     char databuf[DATA_LENGTH];
     int buflen;
     int operation;
     SOCKET client;
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

    void bind_server_cb(std::function<int(int, const char*, int, char*, int)> f)
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
    std::function<int(int, const char*, int, char*, int)> server_cb;

    friend int HandleListen(unsigned short port, void* app);
    friend void HandleRecv(DWORD bytes, LPPER_HANDLE_DATA perHandleData, LPPER_IO_DATA perIoData);
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

int HandleListen(unsigned short port, void* app)
{
    svr_iocp_imp* _this = (svr_iocp_imp*)app;
    int Listen = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
    sys_sock s = sys_sock::attach(Listen);
    LPPER_HANDLE_DATA perDandleData;
    perDandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR,sizeof(PER_HANDLE_DATA));
    perDandleData->socket = Listen;
    CreateIoCompletionPort((HANDLE)Listen,_this->m_hiocp,(ULONG_PTR)perDandleData,0);

    if (!s.bind(port) || !s.listen())
        return INVALID_SOCKET;

    LPFN_ACCEPTEX lpfnAcceptEx = getAcceptEXFun(Listen);

    LPPER_IO_DATA perIoData = (LPPER_IO_DATA)GlobalAlloc(GPTR,sizeof(PER_IO_DATA));
    memset(&(perIoData->overlapped),0,sizeof(OVERLAPPED));
    perIoData->operation = ACCEPT;
    perIoData->client = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,0,0,WSA_FLAG_OVERLAPPED);
    perIoData->buflen = DATA_LENGTH;
    perIoData->app = _this;

    lpfnAcceptEx(Listen, perIoData->client, perIoData->databuf,
                 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16,
                 NULL, &(perIoData->overlapped));

    return s.detach();
}

void HandleAccept(HANDLE CompletionPort, LPPER_HANDLE_DATA perHandleData, LPPER_IO_DATA perIoData)
{
    setsockopt(perIoData->client, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
        (char*)&(perHandleData->socket), sizeof(perHandleData->socket));

    perHandleData->socket = perIoData->client;
    CreateIoCompletionPort((HANDLE)perHandleData->socket,
        CompletionPort,(ULONG_PTR)perHandleData,0);

    memset(&(perIoData->overlapped),0,sizeof(OVERLAPPED));
    perIoData->operation = RECV;
    perIoData->buffer.buf = perIoData->databuf;
    perIoData->buffer.len = perIoData->buflen = DATA_LENGTH;

    DWORD Flags = 0;
    WSARecv(perHandleData->socket, &(perIoData->buffer), 1,
            NULL, &Flags, &(perIoData->overlapped), NULL);
}

void HandleSend(LPPER_HANDLE_DATA perHandleData, LPPER_IO_DATA perIoData)
{
    DWORD Flags = 0;
    perIoData->operation = RECV;
    perIoData->buffer.len = DATA_LENGTH;
    ZeroMemory(&(perIoData->overlapped),sizeof(OVERLAPPED));
    WSARecv(perHandleData->socket, &(perIoData->buffer), 1,
        NULL, &Flags, &(perIoData->overlapped), NULL);
}

void HandleRecv(DWORD bytes, LPPER_HANDLE_DATA perHandleData, LPPER_IO_DATA perIoData)
{
    svr_iocp_imp* _this = (svr_iocp_imp*)perIoData->app;
    if (_this->server_cb)
        bytes = _this->server_cb(perIoData->client, perIoData->buffer.buf,
                         bytes, perIoData->databuf, perIoData->buflen);
    if (bytes > 0)
    {
        perIoData->operation = SEND;
        perIoData->buffer.len = bytes;
        ZeroMemory(&(perIoData->overlapped),sizeof(OVERLAPPED));
        WSASend(perHandleData->socket, &(perIoData->buffer), 1,
            NULL, 0, &(perIoData->overlapped), NULL);
    }
    else
        HandleSend(perHandleData, perIoData);
}

void HandleClose(LPPER_HANDLE_DATA perHandleData, LPPER_IO_DATA perIoData)
{
    closesocket(perHandleData->socket);
    GlobalFree(perHandleData);
    GlobalFree(perIoData);
}

void ServerThread(HANDLE CompletionPort)
{
     DWORD bytes;
     LPPER_HANDLE_DATA perHandleData = NULL;
     LPPER_IO_DATA perIoData;
     while(true)
     {
         bytes = -1;
         GetQueuedCompletionStatus(
             CompletionPort,
             &bytes,
             (PULONG_PTR)&perHandleData,
             (LPOVERLAPPED*)&perIoData,
             INFINITE);

         if(bytes == 0 && (perIoData->operation == RECV ||
             perIoData->operation == SEND))
         {
             HandleClose(perHandleData, perIoData);
             continue;
         }
         if(perIoData->operation == ACCEPT)
         {
             HandleAccept(CompletionPort, perHandleData, perIoData);
             continue;
         }
         if(perIoData->operation == SEND)
         {
             HandleSend(perHandleData, perIoData);
             continue;
         }
         if(perIoData->operation == RECV)
         {
             HandleRecv(bytes, perHandleData, perIoData);
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
    CloseHandle(m_hiocp);
}

void svr_iocp_imp::start()
{
    for(int i=0; i<cpu_count(); i++)
        std::thread(ServerThread,m_hiocp).detach();

    m_sock = HandleListen(m_port, this);
}

void svr_iocp_imp::stop()
{

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

void svr_iocp::bind_server_cb(std::function<int(int, const char*, int, char*, int)> f)
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
