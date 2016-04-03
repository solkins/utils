#ifndef AR_H
#define AR_H

class ar
{
public:
    ar(void* buf):m_buf((char*)buf),m_in(0), m_out(0) {}

    template<typename T>
    ar& operator<<(T v)
    {
        memcpy(m_buf+m_in, &v, sizeof(v));
        m_in += sizeof(v);
        return * this;
    }

    template<typename T>
    ar& operator>>(T& v)
    {
        memcpy(&v, m_buf+m_out, sizeof(v));
        m_out += sizeof(v);
        return * this;
    }

private:
    char* m_buf;
    int m_in, m_out;
};

#endif // AR_H
