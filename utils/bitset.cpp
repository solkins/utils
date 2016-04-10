#include "bitset.h"
#include <string.h>

bitset::bitset(size_t size)
{
    m_data = NULL;
    setsize(size);
}

bitset::bitset(const bitset& orig)
{
    if (m_data)
        delete[] m_data;
    m_total = orig.m_total;
    m_size = orig.m_size;
    if (m_size)
        m_data = new unsigned char[m_size];
    memcpy(m_data, orig.m_data, m_size);
}

bitset::~bitset()
{
    if (m_data)
        delete[] m_data;
}

void bitset::setsize(size_t count)
{
    if (m_data)
        delete[] m_data;
    m_data = NULL;
    
    m_count = 0;
    m_total = count;
    m_size = ((m_total + 7) & (~7)) >> 3;
    if (m_size)
    {
        m_data = new unsigned char[m_size];
        memset(m_data, 0, m_size);
    }
}

size_t bitset::size() const
{
    return m_total;
}

bool bitset::any() const
{
    return m_count > 0;
}

bool bitset::all() const
{
    return m_count == m_total;
}

size_t bitset::count() const
{
    return m_count;
}

bool bitset::none() const
{
    return m_count == 0;
}

void bitset::flip()
{
    for (size_t i = 0; i < m_size; ++i)
        m_data[i] = ~m_data[i];
    m_data[m_size - 1] &= (~0) >> ((m_size >> 3) - m_total);
    m_count = m_total - m_count;
}

void bitset::set(size_t pos, bool val)
{
    bool old = test(pos);
    if (val)
    {
        m_data[pos >> 3] |= (1 << (pos % 8));
        if (!old)
            ++m_count;
    }
    else
    {
        m_data[pos >> 3] &= ~(1 << (pos % 8));
        if (old)
            --m_count;
    }
}

bool bitset::test(size_t pos) const
{
    return (m_data[pos >> 3] >> (pos % 8)) & 1;
}

size_t bitset::first(bool val) const
{
    size_t i;
    for (i = 0; i < m_size; ++i)
    {
        if (m_data[i] != (val ? 0 : 0XFF))
            break;
    }
    size_t pos = (i << 3);
    unsigned char d = m_data[i];
    if (((d & 1) == 1) == val)  return pos;
    if (((d & 2) == 2) == val)  return pos+1;
    if (((d & 4) == 4) == val)  return pos+2;
    if (((d & 8) == 8) == val)  return pos+3;
    if (((d & 16) == 16) == val)  return pos+4;
    if (((d & 32) == 32) == val)  return pos+5;
    if (((d & 64) == 64) == val)  return pos+6;
    if (((d & 128) == 128) == val)  return pos+7;
    return ~0;
}

std::string bitset::to_string() const
{
    char* ret = new char[m_total+1];
    for (size_t i = 0; i < m_total; ++i)
        ret[i] = test(i) ? '1' : '0';
    std::string s = ret;
    delete[] ret;
    return s;
}

void bitset::fromstring(const std::string& str)
{
    setsize(str.size());
    for (size_t i = 0; i < m_total; ++i)
        set(i, str[i] == '1');
}

bool bitset::operator[](size_t pos) const
{
    return test(pos);
}
