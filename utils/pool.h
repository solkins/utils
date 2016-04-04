#ifndef _POOL_H
#define	_POOL_H

#include <queue>
#include <list>
#include <algorithm>
#include "thread.h"

template<typename T>
class pool
{
public:

    pool(int size) : m_semaphore(0, size)
    {
        objsize = 0;
    };

    ~pool()
    {
    };

public:

    void add(T* obj)
    {
        m_mutex.lock();
        idleobjs.push(obj);
        m_mutex.unlock();
        m_semaphore.unlock();
        ++objsize;
    };

    T* remove()
    {
        m_semaphore.lock();
        m_mutex.lock();
        T* obj = idleobjs.front();
        idleobjs.pop();
        m_mutex.unlock();
        --objsize;
        return obj;
    };

    int size()
    {
        return objsize;
    };

    T* getobj()
    {
        m_semaphore.lock();
        m_mutex.lock();
        T* obj = idleobjs.front();
        idleobjs.pop();
        runningobjs.push_back(obj);
        m_mutex.unlock();
        return obj;
    };

    void releaseobj(T* obj)
    {
        m_mutex.lock();
        runningobjs.erase(std::find(runningobjs.begin(), runningobjs.end(), obj));
        idleobjs.push(obj);
        m_mutex.unlock();
        m_semaphore.unlock();
    };

private:
    int objsize;
    std::queue<T*> idleobjs;
    std::list<T*> runningobjs;
    semaphore m_semaphore;
    mutex m_mutex;
};

#endif	/* _POOL_H */

