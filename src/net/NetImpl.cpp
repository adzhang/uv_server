#include "NetImpl.h"
#include "MsgPack.h"

#include <stdlib.h>
#include <assert.h>

#define LOG(fmt, ...) \
    printf("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__);

static void echo_alloc(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf) {
    int writable_sz = 0;
    uv_stream_t* stream = (uv_stream_t*)handle;
    Session* session = (Session*)stream->data;
    Buffer* recv_buf = &session->m_recv_buf;

    buf->base = recv_buf->get_writable_buffer(&writable_sz);
    buf->len = writable_sz >= suggested_size ? suggested_size : writable_sz;
    //LOG("echo_alloc writable_sz: %d\n",buf->len);
}

Session::Session() {
    m_pNet = NULL;
    m_pStream = NULL;
}
Session::~Session() {

}

void Session::after_write(uv_write_t* req, int status) {
    Session* client_socket = (Session*)req->handle->data;

    /* Free the read/write buffer and the request */
    write_req_t* wr = (write_req_t*) req;

    if (status == 0) {
        free(wr);
        return;
    }

    LOG("uv_write error: %s - %s\n", uv_err_name(status), uv_strerror(status));

    if (!uv_is_closing((uv_handle_t*) req->handle))
        client_socket->close(); 
    free(wr);
}

void Session::on_close(uv_handle_t* peer) {
    free(peer);
    uv_stream_t* stream = (uv_stream_t*)peer;
    Session* session = (Session*)stream->data;
    session->m_pStream = NULL;
    printf("session end !!\n");
}

void Session::on_message(const char* packet,int size) {
    //LOG("on_message: %b\n",packet,size);
    sent(packet,size);
}

int Session::sent(const char* data, size_t len) {
    write_req_t* wr = (write_req_t*) malloc(sizeof(write_req_t));
    assert(wr != NULL);
    wr->buf = uv_buf_init(const_cast<char*>(data), len);

    if (uv_write(&wr->req, m_pStream, &wr->buf, 1, after_write)) {
        LOG("uv_write failed\n");
        return 1;
    }
    return 0;
}
void Session::close() {
    uv_close((uv_handle_t*)m_pStream, on_close);
}

int Session::on_recv(char* data, size_t nread) {
    //LOG("has wrote: %d\n",nread);
    m_recv_buf.write((int)nread);    // do write

    int sz=0,ret,merge_ret;
    int skip = MsgPack::get_netpack_protocol_head();

    char* pack_data = m_recv_buf.get_readable_buffer(&sz);
    //LOG("on_recv readable_sz: %d\n",sz);
    if( sz == 0 )
        return 0;

    ret = sub_msg_pack2(pack_data,sz);
    if( ret > 0 ) {                 // full packet
        on_message(pack_data+skip,sz-skip);
        m_recv_buf.read(sz);
    } else if( ret < 0 ) {          // error packet
        close();
    } else {                        // pending packet or ring back
        if( sz >= MsgPack::get_header_len() ) {
            MsgPack* msg_pack = (MsgPack*)pack_data;
            int new_len = msg_pack->get_body_len();
            char* new_data = (char*)malloc(new_len);

            merge_ret = m_recv_buf.readable_merge(new_data,new_len);
            if( merge_ret > 0 ) {
                ret = sub_msg_pack2(new_data,new_len);
                if( ret < 0 ) {
                    close();
                } else {
                    on_message(new_data+skip,new_len-skip);
                    m_recv_buf.read(sz);
                    m_recv_buf.read(new_len-sz);
                }
            }
            free(new_data);
        }
    }

    return 0;
}

void NetImpl::after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    Session* client_handle = (Session*)stream->data;

    if (nread < 0) {
        if( nread == UV_ENOBUFS ) {   // recv_buf is full, recv overload, try to unpack
            client_handle->on_recv(NULL, 0);
        } else if( nread == UV_EOF ) {
            client_handle->close();
        } else
            LOG("after_read unknown error: %d\n", nread);

        return;
    }

    if (nread == 0) {
        return;
    }

    client_handle->on_recv(buf->base, nread);
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
    client_socket->m_pStream = stream;
    client_socket->m_pNet = net;

    r = uv_tcp_init(net->m_pLoop, (uv_tcp_t*)stream);
    assert(r == 0);

    r = uv_accept((uv_stream_t*)&net->m_tcp_server, stream);
    assert(r == 0);

    r = uv_read_start(stream, echo_alloc, after_read);
    assert(r == 0);
}

int NetImpl::init(const char* ip, int port, int max_conn) {
    uv_stream_t* server_stream;
    struct sockaddr_in addr;
    int r;

    assert(0 == uv_ip4_addr(ip, port, &addr));
    m_pLoop = uv_default_loop();

    r = uv_tcp_init(m_pLoop, &m_tcp_server);
    if (r) {
        /* TODO: Error codes */
        LOG("Socket creation error\n");
        return 1;
    }

    r = uv_tcp_bind(&m_tcp_server, (const struct sockaddr*) &addr, 0);
    if (r) {
        /* TODO: Error codes */
        LOG("Bind error\n");
        return 1;
    }
    server_stream = (uv_stream_t*)&m_tcp_server;
    r = uv_listen(server_stream, max_conn, on_connection);
    if (r) {
        /* TODO: Error codes */
        LOG("Listen error %s\n", uv_err_name(r));
        return 1;
    }

    server_stream->data = this;
    return 0;
}

int NetImpl::shutdown() {
    uv_close((uv_handle_t*)&m_tcp_server, NULL);
    return 0;
}

int NetImpl::loop(uint32_t timeout_ms) {
    uv_run(m_pLoop, UV_RUN_DEFAULT);
    return 0;
}


Net* create_net_impl() {
    return new NetImpl();
}
int delete_net(Net* net) {
    delete net;
    return 0;
}
