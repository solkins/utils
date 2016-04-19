#ifndef PC_H
#define PC_H

#include <thread>
#include <mutex>
#include "ring.h"

template<typename T>
class pc_queue
{
public:
    pc_queue(){}

    void put(T* t)
    {
        while (m_ring.full())
            std::this_thread::yield();

        std::lock_guard<std::mutex> lock(m_mutex);
        m_ring.push(t);
    }

    T* get()
    {
        while (m_ring.empty())
            std::this_thread::yield();

        std::lock_guard<std::mutex> lock(m_mutex);
        T* t = m_ring.front();
        m_ring.pop();
        return t;
    }


private:
    std::mutex m_mutex;
    ring<T*> m_ring;
};

#endif // PC_H
