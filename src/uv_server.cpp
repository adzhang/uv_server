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
    Net* net = create_net_impl();
    r = net->init(ip, port, max_conn);
    if(r) {
        printf("\nnet init fail !!\n");
        return 1;
    }
    printf("server launch success, ip: %s port: %d max_conn: %d\n",ip,port,max_conn);
    net->loop(0);
    
    return 0;
}
