#ifndef BUF_H
#define BUF_H

#include <stdint.h>

#include "param.h"

struct CacheBuffer {
    int valid;

    int refcnt;

    int blockn; // block number on disk
    char data[BLOCK_SIZE];
};

struct CacheBuffer* bget(int);
struct CacheBuffer* bread(int);
void bwrite(struct CacheBuffer*);
void brelease(struct CacheBuffer*);

#endif
