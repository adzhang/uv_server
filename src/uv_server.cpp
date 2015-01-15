#include "uv/uv.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv)
{
    struct sockaddr_in addr;

    uv_loop_t* loop = uv_default_loop();
    assert(0 == uv_ip4_addr("127.0.0.1", 9998, &addr));

    printf("loop run !!\n");
    uv_run(loop,UV_RUN_DEFAULT);

    return 0;
}
