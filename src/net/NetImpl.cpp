#include "NetImpl.h"

#include <stdlib.h>
#include <assert.h>

#define LOG(fmt, ...) \
    printf("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__);

static void echo_alloc(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf) {
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}

Session::Session() {
    wr = NULL;
    net = NULL;
    stream = NULL;
}

void Session::after_write(uv_write_t* req, int status) {
    Session* client_socket = (Session*)req->handle->data;

    /* Free the read/write buffer and the request */
    write_req_t* wr = (write_req_t*) req;
    free(wr->buf.base);

    if (status == 0) {
        free(wr);
        return;
    }

    LOG("uv_write error: %s - %s\n", uv_err_name(status), uv_strerror(status));

    if (!uv_is_closing((uv_handle_t*) req->handle))
        client_socket->Close(); 
    free(wr);
}

void Session::on_close(uv_handle_t* peer) {
    free(peer);
}

void Session::OnMessage() {

}
int Session::Sent(char* data, size_t len) {
    wr = (write_req_t*) malloc(sizeof(write_req_t));
    assert(wr != NULL);
    wr->buf = uv_buf_init(data, len);

    if (uv_write(&wr->req, stream, &wr->buf, 1, after_write)) {
        LOG("uv_write failed");
        return 1;
    }
    return 0;
}
void Session::Close() {
    uv_close((uv_handle_t*)stream, on_close);
}

int Session::OnRecv(char* data, size_t nread) {
    LOG("recv message: %s",data);
    Sent(data,nread);
    return 0;
}

void NetImpl::after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    Session* client_handle = (Session*)stream->data;

    if (nread < 0) {
        /* Error or EOF */
        assert(nread == UV_EOF);

        if (buf->base) {
            free(buf->base);
        }

        client_handle->Close();
        return;
    }

    if (nread == 0) {
        /* Everything OK, but nothing read. */
        free(buf->base);
        return;
    }

    client_handle->OnRecv(buf->base, nread);
}


void NetImpl::on_connection(uv_stream_t* server_stream, int status) {
    printf("new connection !!\n");
    NetImpl* net = (NetImpl*)server_stream->data;
    uv_stream_t* stream;
    Session* client_socket;
    int r;

    if (status != 0) {
        LOG("Connect error %s\n", uv_err_name(status));
    }
    assert(status == 0);

    client_socket = new Session();
    stream = (uv_stream_t*)malloc(sizeof(uv_tcp_t));
    stream->data = client_socket;
    client_socket->stream = stream;
    client_socket->net = net;

    r = uv_tcp_init(net->loop, (uv_tcp_t*)stream);
    assert(r == 0);

    r = uv_accept((uv_stream_t*)&net->tcpServer, stream);
    assert(r == 0);

    r = uv_read_start(stream, echo_alloc, after_read);
    assert(r == 0);
}

int NetImpl::Init(const char* ip, int port, int max_conn) {
    uv_stream_t* server_stream;
    struct sockaddr_in addr;
    int r;

    assert(0 == uv_ip4_addr(ip, port, &addr));
    loop = uv_default_loop();

    r = uv_tcp_init(loop, &tcpServer);
    if (r) {
        /* TODO: Error codes */
        LOG("Socket creation error\n");
        return 1;
    }

    r = uv_tcp_bind(&tcpServer, (const struct sockaddr*) &addr, 0);
    if (r) {
        /* TODO: Error codes */
        LOG("Bind error\n");
        return 1;
    }
    server_stream = (uv_stream_t*)&tcpServer;
    r = uv_listen(server_stream, max_conn, on_connection);
    if (r) {
        /* TODO: Error codes */
        LOG("Listen error %s\n", uv_err_name(r));
        return 1;
    }

    server_stream->data = this;
    return 0;
}

int NetImpl::Shutdown() {
    uv_close((uv_handle_t*)&tcpServer, NULL);
    return 0;
}

int NetImpl::Loop(uint32_t timeout_ms) {
    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}


Net* CreateNetImpl() {
    return new NetImpl();
}
int DeleteNet(Net* net) {
    delete net;
    return 0;
}
