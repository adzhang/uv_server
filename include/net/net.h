#ifndef _NET_H_
#define _NET_H_

#include <stdio.h>
#include <stdint.h>

class Handle {
public:
    Handle(){}
    virtual ~Handle(){}
    virtual void on_message(const char* packet,int size) = 0;
    virtual int sent(const char* data, size_t len) = 0;
    virtual void close() = 0;
};

class Net {
public:
    Net(){}
    virtual ~Net(){}
    virtual int init(const char* ip, int port, int max_conn) = 0;
    virtual int shutdown() = 0;
    virtual int loop(uint32_t timeout_ms) = 0;
};

Net* create_net_impl();
int delete_net(Net* net);

#endif // _NET_H_

