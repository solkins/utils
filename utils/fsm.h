/*
 * sample:
 *
 * enum state{stopped, playing, paused};
 * enum event{play, pause, stop};
 * fsm f(paused, stop);
 * f.setevent(play, stopped, playing);
 * f.setevent(play, paused, playing);
 * f.setevent(pause, playing, paused);
 * f.setevent(pause, paused, playing);
 * f.setevent(stop, playing, stopped);
 * f.setevent(stop, paused, stopped);
 * f.bindaction(stopped, []{qDebug()<<"stopped";});
 * f.bindaction(playing, []{qDebug()<<"playing";});
 * f.bindaction(paused, []{qDebug()<<"paused";});
 * f.handle(play);
 * f.handle(pause);
 * f.handle(pause);
 * f.handle(stop);
 *
 */

#ifndef FSM_H
#define FSM_H

#include <deque>
#include <functional>

class fsm
{
public:
    fsm(int maxstate, int maxevent);
    ~fsm();

    void setevent(int event, int beginstate, int endstate);
    void bindaction(int state, std::function<void(void)> f);

    void handle(int event);

private:
    int m_state;
    int* maptable;
    int row, col;
    std::deque<std::function<void(void)>> actions;
};

#endif // FSM_H
