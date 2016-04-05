#ifndef THREADPOOL_H
#define	THREADPOOL_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>
#include "sysinfo.h"
#include "singleton.h"

class threadpool
{
    typedef std::function<void()> Task;

public:
    threadpool(int size = 0)
    {
#ifdef _MSC_VER
		_Cnd_do_broadcast_at_thread_exit();	// correct static std::thread variable will block at std::thread::join after main exit
#endif
		if (size == 0)
            size = cpu_count() * 2;
        running = true;
        for (int i = 0; i < size; ++i)
            _threads.push_back(std::thread(&threadpool::threadproc, this));
    }

    ~threadpool()
    {
        running = false;
        _notEmpty.notify_all();
        for (auto &iter : _threads)
            iter.join();
    }

    template<typename F, typename... A>
    void run(F&& f, A&&... a)
    {
        std::unique_lock<std::mutex> ul(_mtx);
        _tasks.push_back(std::bind(f, a...));
        _notEmpty.notify_one();
    }

    int size() const
    {
        return _threads.size();
    }

private:
    void threadproc()
    {
        while (running)
        {
            Task task(take());
            if (task)
                task();
        }
    }

    Task take()
    {
        std::unique_lock<std::mutex> ul(_mtx);
        if (running && _tasks.empty())
            _notEmpty.wait(ul);

        if (!_tasks.empty())
        {
            Task task = _tasks.front();
            _tasks.pop_front();
            return task;
        }
        return (nullptr);
    }

private:
    std::atomic<bool> running;
    std::mutex _mtx;
    std::condition_variable _notEmpty;
    std::deque<std::thread> _threads;
    std::deque<Task> _tasks;
};

template<typename F, typename... A>
static void threadpoolrun(F&& f, A&&... a)
{
    singleton<threadpool>::instance().run(f, a...);
}

static int threadpoolsize()
{
    return singleton<threadpool>::instance().size();
}

#endif	/* THREADPOOL_H */
