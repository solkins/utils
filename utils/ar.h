#ifndef AR_H
#define AR_H

#include <memory.h>
#include <string.h>

class ar
{
public:
    ar(void* buf):m_buf((char*)buf),m_in(0), m_out(0) {}

    int readlen() const
    {
        return m_out;
    }

    int writelen() const
    {
        return m_in;
    }

    ar& read(void* data, int len)
    {
        memcpy(data, m_buf+m_out, len);
        m_out += len;
        return *this;
    }

    ar& write(const void* data, int len)
    {
        memcpy(m_buf+m_in, data, len);
        m_in += len;
        return *this;
    }

    ar& operator<<(const char* s)
    {
        return write(s, strlen(s)+1);
    }

    ar& operator>>(char* s)
    {
        return read(s, strlen(m_buf+m_out)+1);
    }

    template<typename T>
    ar& operator<<(T v)
    {
        return write(&v, sizeof(v));
    }

    template<typename T>
    ar& operator>>(T& v)
    {
        return read(&v, sizeof(v));
    }

private:
    char* m_buf;
    int m_in, m_out;
};

#endif // AR_H
