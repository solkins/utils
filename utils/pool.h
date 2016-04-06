#ifndef POOL_H
#define	POOL_H

#include <queue>
#include <mutex>

template<typename T>
class pool
{
public:
    class ptr
    {
        friend class pool;
    private:
        ptr(pool<T>* p, T* t) : _pool(p), _ptr(t) {}

    public:
        ~ptr()
        {
            std::lock_guard<std::mutex> ul(_pool->_mtx);
            _pool->idles.push(_ptr);
        }

        T* operator->()
        {
            return _ptr;
        }

    private:
        pool<T>* _pool;
        T* _ptr;
    };

private:
    pool(){}

    ~pool()
    {
        while (!idles.empty())
        {
            T* it = idles.front();
            idles.pop();
            delete it;
        }
    }

public:
    static pool& instance()
    {
        static pool p;
        return p;
    }

    template<typename... A>
    void init(int size, A&&... a)
    {
        for (int i = 0; i < size; ++i)
            idles.push(new T(a...));
    }

    ptr get()
    {
        std::lock_guard<std::mutex> ul(_mtx);
        T* it = idles.front();
        idles.pop();
        return ptr(this, it);
    }

private:
    std::queue<T*> idles;
    std::mutex _mtx;
};

#endif	/* POOL_H */

