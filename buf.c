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
//     follow on last recently buffer

#include <stdint.h>
#include <stdio.h>

#include "disk_emulator.h"
#include "buf.h"
#include "param.h"


struct CacheBuffer buf[BCACHE_NUM];

/*
 * Scan through memory array and find demanding block
 */
struct CacheBuffer* 
bget(int blockn)
{
    printf("bget: get by block number: %d\n", blockn);
    //Scan buffer array, if find return, else allocate a new one
    struct CacheBuffer *b;

    for (b = buf; b < &buf[BCACHE_NUM]; b++) {
        if (b->blockn == blockn) {
            b->refcnt++;
            printf("bget: find demanding block: %d\n", blockn);
            return b;
        }
    }
    
    // dont find demanding block, allocate a new one
    for (b = buf; b < &buf[BCACHE_NUM]; b++) {
        if (b->refcnt == 0) {
            b->blockn = blockn;
            b->valid = 0;
            b->refcnt = 1;
            printf("bget: allocate new block for: %d\n", blockn);
            return b;
        }
    }
    printf("ERROR: can't allocate new free cache buffer\n");
}

/*
 * return valid cache buffer or allocate new one
 */
struct CacheBuffer* 
bread(int blockn)
{
    printf("bread: read by block number\n");
    struct CacheBuffer* b;
    
    b = bget(blockn);
    if (!b->valid) {
        // read into buffer from disk
        read_block(blockn, 1, b->data); 
        b->valid = 1;
    }
    return b;
}

void
bwrite(struct CacheBuffer *b)
{
    write_block(b->blockn, b->data, sizeof(b->data));
}

void
brelease(struct CacheBuffer *b)
{
    b->refcnt--;
}

