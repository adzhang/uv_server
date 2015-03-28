#include "timer_mng.h"

#include <stdlib.h>

TimerMng::TimerMng(uv_loop_t* loop) {
    m_pLoop = loop;
}

TimerMng::~TimerMng() {

}

static void uv_timer_callback(uv_timer_t* handle) {
    TimerState* state = (TimerState*)handle->data;
    int ret = state->handle(state->data, state->data2);
    if( ret == 0 ) {
        uv_close((uv_handle_t*)handle, NULL);
        free(handle->data);
        free(handle);
    }else {
        if( state->expires > 0 )
            state->expires = 0;
        uv_timer_start(handle, uv_timer_callback, state->cycle, 0);
    }
}

int TimerMng::add_timer( unsigned long expires, unsigned long id, unsigned long data, unsigned long data2, unsigned long cycle, timer_func handler ) {
    uv_timer_t* handle = (uv_timer_t*)malloc(sizeof(uv_timer_t));
    int ret = uv_timer_init(m_pLoop, handle);
    if( ret < 0 ) {
        free(handle);
        return -1;
    }

    TimerState* timer_state = (TimerState*)malloc(sizeof(TimerState));
    timer_state->expires = expires;
    timer_state->data = data;
    timer_state->data2 = data2;
    timer_state->cycle = cycle;
    timer_state->handle = handler;

    handle->data = timer_state;
    if( expires > 0 )
        ret = uv_timer_start(handle, uv_timer_callback, expires, 0);
    else
        ret = uv_timer_start(handle, uv_timer_callback, cycle, 0);

    if( ret < 0 ) {
        free(handle);
        free(timer_state);
        return -1;
    }
    return 1;
}

int TimerMng::del_timer( int index ) {

    return 1;
}

int TimerMng::mod_timer( int index, unsigned long expires ) {

    return 1;
}


