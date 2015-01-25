#ifndef _MSGPACK_H_
#define _MSGPACK_H_

#define NET_PACK_PROTOCOL_HEAD 0
#define MAX_MSG_PACK_LEN 4096 

class MsgPack {
public:
    MsgPack(unsigned short c = 0) {
        cmd = c;
    }

    static int get_netpack_protocol_head() { return NET_PACK_PROTOCOL_HEAD; } 

    //static int get_header_len() { return 2+2; }
    static int get_header_len() { return 0; }
    int get_body_len() { return len; }

public:
    unsigned short  len;
    unsigned short  cmd;
};

// netpack protocol: { len(short), body }
int sub_msg_pack(const char* data, int len) {
    if( len >= sizeof(MsgPack) ) {
        const MsgPack* pack = (MsgPack*)data;
        int pack_len = (int)pack->len;

        if( pack_len > MAX_MSG_PACK_LEN ) {
            return -1;
        }

        if( len >= pack_len ) {
            return pack_len;
        }
    }

    return 0;
}

int sub_msg_pack2(const char* data, int len) {
    if( len > 0 )
        return len;
    else
        return 0;
}


#endif  // _MSGPACK_H_
