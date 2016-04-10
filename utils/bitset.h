#ifndef _BITSET_H
#define	_BITSET_H

#include <string>

typedef unsigned char   uint8;

class bitset
{
public:
    bitset();
    bitset(size_t size = 0);
    bitset(const bitset& orig);
    ~bitset();

    void setsize(size_t count);
    size_t size() const;
    bool any() const;
    bool all() const;
    size_t count() const;
    bool none() const;
    void flip();
    void set(size_t pos, bool val = true);
    bool test(size_t pos) const;
    size_t first(bool val = true) const;
    std::string to_string() const;
    void fromstring(const std::string& str);
    
    bool operator[](size_t pos) const;

private:
    size_t m_count;
    size_t m_total;
    size_t m_size;
    uint8* m_data;
};

#endif	/* _BITSET_H */

