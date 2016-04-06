#ifndef RINGQUEUE_H
#define RINGQUEUE_H

template<class T>
class ring
{
public:
    ring()
    {
        clear();
    }

    void push(const T& t)
    {
        if (!full())
            _ring[tail++] = t;
    }

    void pop()
    {
        if (tail != head)
            ++head;
    }

    T front()
    {
        return _ring[head];
    }

    bool full()
    {
        return (unsigned char)(tail + 1) == head;
    }

    bool empty()
    {
        return tail == head;
    }

    unsigned char size()
    {
        return tail - head;
    }

    void clear()
    {
        head = tail = 0;
    }

private:
    T _ring[256];
    unsigned char head, tail;
};

#endif /* RINGQUEUE_H */
