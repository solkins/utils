#ifndef FILTER_H
#define FILTER_H

#include <functional>
#include <iostream>
#include <list>

class filter
{
public:
    filter():next(nullptr){}
    virtual void start(){ if(next) next->start(); }
    virtual void write(const void* data, int len){ if(next) next->write(data, len); }
    virtual void end(){ if(next) next->end(); }

    void setnext(filter* f){ next = f; }

private:
    filter* next;
};

std::istream& operator>>(std::istream& is, filter& f);

class output : public filter
{
public:
    output(std::ostream& ost = std::cout) : filter(), _out(ost) {}

    virtual void write(const void* data, int len);

private:
    std::ostream& _out;
};

class fork : public filter
{
public:
    fork(filter* f1, filter* f2, filter* f3 = nullptr, filter* f4 = nullptr);
    void addnext(filter* f);

    virtual void start();
    virtual void write(const void* data, int len);
    virtual void end();

private:
    std::list<filter*> sink;
};

class chain : public filter
{
public:
    chain(filter* f1, filter* f2, filter* f3 = nullptr, filter* f4 = nullptr);
    void addnext(filter* f);

private:
    filter* tail;
};

class Tfilter : public filter
{
public:
    Tfilter(std::function<int(const void*, int, void*, int)> w);
    Tfilter(std::function<void()> s, std::function<int(const void*, int, void*, int)> w, std::function<void()> e);
    virtual void start();
    virtual void write(const void* data, int len);
    virtual void end();

private:
    std::function<void()> _start;
    std::function<int(const void*, int, void*, int)> _write;
    std::function<void()> _end;
    char buf[BUFSIZ];
};

#endif // FILTER_H
