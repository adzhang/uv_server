#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "MsgPack.h"
#include "string.h"

#define MAX_BUFFER_DATA_LEN (MAX_MSG_PACK_LEN * 4)

class Buffer {
public:
    Buffer() { reset(); }
    void    reset() { head_=tail_=data_; }

    char*   get_writable_buffer( int* buf_size );
    int     get_writable_buffer_size( void );
    char*   get_readable_buffer( int* buf_size );
    int     get_readable_buffer_size( void );

    void    check_end();
    int     read(int len);
    int     write(int len);

    int     readable_merge(char* new_buf, int len);

public:
    char    *head_,*tail_;
    char    data_[MAX_BUFFER_DATA_LEN];
};

inline char* Buffer::get_writable_buffer( int* buf_size )
{   
    *buf_size = get_writable_buffer_size();
    return tail_;   
}

inline int Buffer::get_writable_buffer_size( void )
{   
    int buf_size = 0;
    if( tail_ >= head_ )
        buf_size = (int)(data_ + sizeof(data_) - tail_);
    else
        buf_size = (int)(head_ - tail_);

    // tail never reach head
    if( tail_ < head_ || head_ == data_ )
        buf_size--;

    return buf_size;
}

inline char* Buffer::get_readable_buffer( int* buf_size )
{   
    *buf_size = get_readable_buffer_size();
    return head_;   
}

inline int Buffer::get_readable_buffer_size( void )
{   
    int readable_sz = 0;
    if( head_ < tail_ )
        readable_sz = (int)(tail_ - head_);
    else if( head_ > tail_ )
        readable_sz = (int)(data_ + sizeof(data_) - head_);

    return readable_sz; 
}
inline void Buffer::check_end() {
    char* buf_end = data_ + sizeof(data_);
    if( head_ == buf_end )
        head_ = data_;
    if( tail_ == buf_end )
        tail_ = data_;

    if( head_ == tail_ )        // no readable data, reset
        reset();
}

inline int Buffer::read(int len) {
    int nread = 0;
    int readable_sz = get_readable_buffer_size();
    if( len <= readable_sz )
        nread = len;
    else
        nread = readable_sz;

    head_ += nread;
    check_end();
    return nread;
}

inline int Buffer::write(int len) {
    int nwrite = 0;
    int buf_size = get_writable_buffer_size();
    if( len <= buf_size )
        nwrite = len;
    else 
        nwrite = buf_size;

    tail_ += nwrite;
    check_end();
    return nwrite;
}

// merge data from tail to head
int Buffer::readable_merge(char* new_buf, int len) {
    int end_sz, begin_sz;
    char* buf_end = data_ + sizeof(data_);
    int act_readable = 0;
    if( head_ > tail_ ) {
        end_sz = (int)(buf_end - head_);
        begin_sz = (int)(tail_ - data_);
        act_readable = end_sz + begin_sz;
        if( act_readable >= len ) {
            memcpy(new_buf,head_,end_sz);
            memcpy(new_buf+end_sz,tail_,begin_sz);
            return len;
        } else 
            return -1;
    }
    return -1;
}

#endif  // _BUFFER_H_
