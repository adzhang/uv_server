#ifndef _TIMER_MNG_H_
#define _TIMER_MNG_H_

#include "uv/uv.h"

typedef int ( *timer_func )( int p1, int p2 );

struct TimerState {
    unsigned long expires;
    unsigned long data;
    unsigned long data2;
    unsigned long cycle;
    timer_func handle;
};

class TimerMng {
public:
    TimerMng(uv_loop_t* loop);
    ~TimerMng();

    int add_timer( unsigned long expires, unsigned long id, unsigned long data, unsigned long data2, unsigned long cycle, timer_func handler = 0 );
    int del_timer( int index );
    int mod_timer( int index, unsigned long expires );

private:
    uv_loop_t* m_pLoop;
};


#endif
