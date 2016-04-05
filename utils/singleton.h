#ifndef _SINGLETON_H
#define SINGLETON_H

template <typename T>
class singleton : private T
{
private:
    singleton() {}
    ~singleton() {}

public:
    static T &instance()
    {
        static singleton<T> inst;
        return inst;
    }
};

#endif // SINGLETON_H
