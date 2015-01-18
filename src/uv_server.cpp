#include "uv/uv.h"
#include "net/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv)
{
    int r;
    char* ip = "127.0.0.1";
    int port = 9998;
    int max_conn = 128;
    Net* net = CreateNetImpl();
    r = net->Init(ip, port, max_conn);
    if(r) {
        printf("\nnet Init fail !!\n");
        return 1;
    }
    printf("server launch success, ip: %s port: %d max_conn: %d\n",ip,port,max_conn);
    net->Loop(0);
    
    /*
    struct sockaddr_in addr;

    uv_loop_t* loop = uv_default_loop();
    assert(0 == uv_ip4_addr("127.0.0.1", 9998, &addr));

    printf("loop run !!\n");
    uv_run(loop,UV_RUN_DEFAULT);
    */

    return 0;
}
