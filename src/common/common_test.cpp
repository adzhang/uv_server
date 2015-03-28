extern "C" {
#include "ringbuffer.h"
}

#include <time.h>
#include <assert.h>  /* assert */
#include <stdio.h>  /* fprintf */
#include <stdlib.h>  /* malloc */
#include <string.h>  /* memcpy */
#include <unistd.h>
#include <pthread.h>

#define TEST_DATA_SIZE 256 * 1024 * 1024
#define ASSERT(e) \
    if (!(e)) {\
      fprintf(stderr, "ASSERT: " #e " failed on %d\n", __LINE__); \
      abort(); \
    }

static char* data;
static ringbuffer rb;

int rw_test() {
  int i;
  int j;
  int r;
  size_t len;
  char* ptr;

  data = (char*)malloc(TEST_DATA_SIZE);
  assert(data != NULL);
  ringbuffer_init(&rb);

  /* Fill test data */
  for (i = 0; i < TEST_DATA_SIZE; i++)
    data[i] = (i * i) % 137;

  /* Fill ringbuffer */
  i = 0;
  while (i < TEST_DATA_SIZE) {
    len = TEST_DATA_SIZE - i;
    ptr = ringbuffer_write_ptr(&rb, &len);
    ASSERT(ptr != NULL);

    /* Always make progress */
    ASSERT(len > 0);

    memcpy(ptr, data + i, len);
    i += len;
    r = ringbuffer_write_append(&rb, len);
    ASSERT(r == 0);
  }
  ASSERT(ringbuffer_size(&rb) == TEST_DATA_SIZE);

  /* Read from it */
  i = 0;
  while (i < TEST_DATA_SIZE) {
    len = TEST_DATA_SIZE - i;
    ptr = ringbuffer_read_next(&rb, &len);
    ASSERT(ptr != NULL);

    /* Always make progress */
    ASSERT(len > 0);

    for (j = 0; j < len; j++)
      ASSERT(ptr[j] == data[i + j]);

    ringbuffer_read_skip(&rb, len);
    i += len;
  }

  /* Destroy it */
  ringbuffer_destroy(&rb);

  return 0;
}

char next_letter(char c) {
    if( c >= 'a' && c < 'z' )
        return c+1;
    return 'a';
}

static int s_count = 1000000;
void *writer(void *args) {
    int r,i=0,j,count=0;
    size_t len;
    char c = 'a';
    char* ptr;
    while( i < s_count ) {
        len = rand() % 10;
        ptr = ringbuffer_write_ptr(&rb, &len);
        if( ptr == NULL ) {
            printf("\nringbuffer_write_ptr ptr NULL\n");
            sleep(0);
            continue;
        }

        if( len <= 0 ) {
            printf("\nringbuffer_write_ptr len <= 0\n");
            sleep(0);
            continue;
        }

        len = count+len >= s_count ? s_count-count : len;
        for( j=0 ; j<len ; ++j ) {
            ptr[j] = c;
            ++count;
            c = next_letter(c);
        }

        i += len;
        r = ringbuffer_write_append(&rb, len);
        ASSERT(r == 0);
    }
    printf("\nwriter end, write: %d\n", count);
}

int thread_safe_test() {
    int err;
    int i=0,j,count=0,split=1;
    size_t len;
    char* ptr;
    pthread_t pid;

    ringbuffer_init(&rb);
    srand((int)time(0));
    printf("\n");

    err = pthread_create(&pid, NULL, writer, NULL);
    if( err != 0 ) {
        printf("cannot create pthread, err: %d\n",err);
        return 1;
    }
    sleep(0);
    while(i < s_count) {
        ptr = ringbuffer_read_next(&rb, &len);
        if(ptr == NULL) {
            //printf("\nringbuffer_read_next ptr NULL\n");
            sleep(0);
            continue;
        }

        if( len <= 0 ) {
            //printf("\nringbuffer_read_next len <= 0\n");
            sleep(0);
            continue;
        }

        for( j=0 ; j<len; ++j ) {
            printf("%c",ptr[j]);
            ++count;
            if( ptr[j] == 'z' ) {
                printf("\t");
                if( split++ % 4 == 0 )
                    printf("\n");
            }
        }

        ringbuffer_read_skip(&rb, len);
        i += len;
    }
    printf("\nreader end, read %d\n",count);
    pthread_join(pid,NULL);
    return 0;
}

int test_main() {
    int r;
    // 读写测试
    /*
    r = rw_test();
    if(r) {
        printf("rw_test() failed !!\n");
        return 1;
    }
    */

    // 单读者单写者线程安全测试
    r = thread_safe_test();
    if(r) {
        printf("thread_safe_test() failed !!\n");
        return 1;
    }

    return 0;
}
