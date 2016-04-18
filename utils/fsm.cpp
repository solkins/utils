#include "fsm.h"
#include <memory.h>

fsm::fsm(int maxstate, int maxevent)
{
    m_state = 0;
    row = maxstate + 1;
    col = maxevent + 1;
    maptable = new int[row * col];
    memset(maptable, -1, sizeof(int) * row * col);
    for (int i=0; i<row; ++i)
        actions.push_back(std::function<void(void)>());
}

fsm::~fsm()
{
    delete[] maptable;
    actions.clear();
}

void fsm::setevent(int event, int beginstate, int endstate)
{
    maptable[row * beginstate + event] = endstate;
}

void fsm::bindaction(int state, std::function<void(void)> f)
{
    actions[state] = f;
}

void fsm::handle(int event)
{
    int newstate = maptable[row * m_state + event];
    if (newstate != -1)
    {
        m_state = newstate;
        actions[m_state]();
    }
}
