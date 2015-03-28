#include "uv/uv.h"
#include "net/net.h"
#include "common/timer_mng.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int timer_cb( int p1, int p2 ) {
    time_t now = time(NULL);
    struct tm* current_time = localtime(&now); 
    printf("[%d-%d-%d %d:%d:%d] ------- timer_cb %d\n",
            1900 + current_time->tm_year,
            1 + current_time->tm_mon,
            current_time->tm_mday,
            current_time->tm_hour,
            current_time->tm_min,
            current_time->tm_sec,
            p1);
    return 1;
}

int main(int argc, char** argv)
{
    int r;
    char* ip = "127.0.0.1";
    int port = 9998;
    int max_conn = 128;
    Net* net = create_net_impl();
    r = net->init(ip, port, max_conn);
    if(r) {
        printf("\nnet init fail !!\n");
        return 1;
    }
    printf("server launch success, ip: %s port: %d max_conn: %d\n",ip,port,max_conn);

    TimerMng* timer_mng = new TimerMng(uv_default_loop());
    timer_mng->add_timer( 1000, 1, 10, 0, 2000, timer_cb );

    net->loop(0);
    
    return 0;
}
