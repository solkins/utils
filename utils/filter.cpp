#include "filter.h"
#include <iostream>

std::istream& operator>>(std::istream& is, filter& f)
{
    f.start();
    char buf[BUFSIZ];
    while (is.good())
    {
        is.read(buf, BUFSIZ);
        f.write(buf, is.gcount());
    }
    f.end();
    return is;
}

void output::write(const void *data, int len)
{
    std::cout.write((char*)data, len);
}

fork::fork(filter *f1, filter *f2, filter *f3, filter *f4) : filter()
{
    addnext(f1);
    addnext(f2);
    if (f3) addnext(f3);
    if (f4) addnext(f4);
}

void fork::addnext(filter *f)
{
    sink.push_back(f);
}

void fork::start()
{
    for (auto f : sink)
        f->start();
}

void fork::write(const void *data, int len)
{
    for (auto f : sink)
        f->write(data, len);
}

void fork::end()
{
    for (auto f : sink)
        f->end();
}

chain::chain(filter *f1, filter *f2, filter *f3, filter *f4) : filter(), tail(nullptr)
{
    addnext(f1);
    addnext(f2);
    if (f3) addnext(f3);
    if (f4) addnext(f4);
}

void chain::addnext(filter *f)
{
    if (tail)
        tail->setnext(f);
    else
        setnext(f);

    tail = f;
}

Tfilter::Tfilter(std::function<int(const void*, int, void*, int)> w) : filter()
{
    _write = w;
}

Tfilter::Tfilter(std::function<void()> s, std::function<int(const void*, int, void*, int)> w, std::function<void()> e) : filter()
{
    _start = s;
    _write = w;
    _end = e;
}

void Tfilter::start()
{
    if (_start) _start();
    filter::start();
}

void Tfilter::write(const void* data, int len)
{
    if (_write)
    {
        int retlen = _write(data, len, buf, BUFSIZ);
        if (retlen > 0)
            filter::write(buf, retlen);
    }
}

void Tfilter::end()
{
    if (_end) _end();
    filter::end();
}
