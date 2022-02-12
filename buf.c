// Disk simple array of buffer caches
// Buffer cache actually hold information of one disk block
// Cache work by simple principle: LRU

// binit -- init buffer cache
// bread -- read a disk block
// bwrite -- write a disk block
// brelease -- release disk block

// TODO:
// *** Need interface to sd card, emulate it with array and some calls
// *** Start with simple buffer cache, double linked list
// *** Think about how double linked list should be init so we can
// follow on last recently buffer
#include <stdint.h>

#include "defs.h"
#include "param.h"
#include "buf.h"

struct {
    struct CacheBuffer buf[BCACHE_NUM];
} bcache;

// Init cache buffer
// For init locks on buffers
void
binit(void)
{
    struct CacheBuffer *b;

    for (b = bcache.buf; b < &bcache.buf[BCACHE_NUM]; b++) {
    }
}

// Scan through array and find 
struct CacheBuffer* 
bget(uint32_t dev, uint32_t blockn)
{
    //Scan buffer array, if find return, else allocate a new one
    struct CacheBuffer *buf;


    for (buf = bcache.buf; buf < &bcache.buf[BCACHE_NUM]; buf++) {
        // find block with same dev and block num
        if (buf->dev == dev && buf->blockn == blockn) {
            buf->refcnt++;
            return buf;
        }
    }
    
    // dont find demanding block, allocate a new one
    for (buf = bcache.buf; buf < &bcache.buf[BCACHE_NUM]; buf++) {
        if (buf->refcnt == 0) {
            kprintf("Current data is %s,   %d\n", buf->data, blockn);
            buf->dev = dev;
            buf->blockn = blockn;
            buf->valid = 0;
            buf->refcnt = 1;
            return buf;
        }
    }
    panic("can't allocate new free cache buffer");
}

// Return buffer with actual data
struct CacheBuffer* 
bread(uint32_t dev, uint32_t blockn)
{
    struct CacheBuffer* buf;
    
    // call bget, if it's new, call read routine from sd card
    buf = bget(dev, blockn);
    if (!buf->valid) {
        // read into buffer from disk
        read_disk(blockn, 1, buf->data); 
        buf->valid = 1;
    }
    return buf;
}

void
bwrite(struct CacheBuffer *buf)
{
    struct CacheBuffer *b;

    // Write buffer content into sd card
    write_disk(buf->blockn, buf->data, sizeof(buf->data));
}

void
brelease(struct CacheBuffer *buf)
{
    buf->refcnt--;
}
