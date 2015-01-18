#include "net/net.h"
#include "uv/uv.h"

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

class NetImpl;
class Session :public Handle {
public:
    Session();
    virtual ~Session() {}

    virtual void OnMessage();
    virtual int Sent(char* data, size_t len);
    virtual void Close();

    int OnRecv(char* data, size_t nread);

    static void after_write(uv_write_t* req, int status);
    static void on_close(uv_handle_t* peer);

    write_req_t* wr;
    NetImpl* net;
    uv_stream_t* stream;
};

class NetImpl :public Net {
public:
    NetImpl() {}
    virtual ~NetImpl() {}
    
    virtual int Init(const char* ip, int port, int max_conn);
    virtual int Shutdown();
    virtual int Loop(uint32_t timeout_ms);

    static void after_read(uv_stream_t*, ssize_t nread, const uv_buf_t* buf);
    static void on_connection(uv_stream_t*, int status);

    uv_tcp_t tcpServer;
    uv_loop_t* loop;
};


