#ifndef _NET_H_
#define _NET_H_

#include <stdio.h>
#include <stdint.h>

class Handle {
public:
    Handle(){}
    virtual ~Handle(){}
    virtual void OnMessage() = 0;
    virtual int Sent(char* data, size_t len) = 0;
    virtual void Close() = 0;
};

class Net {
public:
    Net(){}
    virtual ~Net(){}
    virtual int Init(const char* ip, int port, int max_conn) = 0;
    virtual int Shutdown() = 0;
    virtual int Loop(uint32_t timeout_ms) = 0;
};

Net* CreateNetImpl();
int DeleteNet(Net* net);

#endif // _NET_H_

