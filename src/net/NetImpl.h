#ifndef _NETIMPL_H_
#define _NETIMPL_H_

#include "net/net.h"
#include "uv/uv.h"
#include "Buffer.h"

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

class NetImpl;
class Session :public Handle {
public:
    Session();
    virtual ~Session();

    virtual void on_message(const char* packet,int size);
    virtual int sent(const char* data, size_t len);
    virtual void close();

    int on_recv(char* data, size_t nread);

    static void after_write(uv_write_t* req, int status);
    static void on_close(uv_handle_t* peer);

    NetImpl* m_pNet;
    uv_stream_t* m_pStream;
    Buffer  m_recv_buf;
};

class NetImpl :public Net {
public:
    NetImpl() {}
    virtual ~NetImpl() {}
    
    virtual int init(const char* ip, int port, int max_conn);
    virtual int shutdown();
    virtual int loop(uint32_t timeout_ms);

    static void after_read(uv_stream_t*, ssize_t nread, const uv_buf_t* buf);
    static void on_connection(uv_stream_t*, int status);

    uv_tcp_t m_tcp_server;
    uv_loop_t* m_pLoop;
};

#endif  // _NETIMPL_H_
