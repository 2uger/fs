/*
 * Simple file system
 * Basic structure on disk:
 * SPB :: INODES :: BITMAP :: BLOCKS
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "buf.h"
#include "inode.h"


struct spblock spb;


void
readspblock(struct spblock *spb)
{
    struct CacheBuffer *b;
    b = bread( 0);
    memmove(spb, b->data, sizeof(struct spblock));
    brelease(b);
}

static void
zeroblock(int blockn)
{
    struct CacheBuffer *b;
    b = bread(blockn);
    memset(b, 0, BLOCK_SIZE);
    bwrite(b);
    brelease(b);
}

// Functions to work with blocks on disk
// balloc - allocating block on disk
//          setup right bitmap bit
// bfree - clean disk data block by setting bitmap bit into zero

/*
 * allocating new block on disk
 */
int
balloc()
{
    int m, b, bm;
    struct CacheBuffer *buf;

    // start from 1 to skip super block
    // incrementing by BPB to skip receiving the same bitmap block
    for (b = 1; b < spb.size; b += BPB) {
        // get block of BPB*BLOCK_SIZE bits showing status of blocks
        buf = bread(BMBLOCK(b, spb));
        // iterating through byte till BPB or untill blocks amount on disk
        for (bm = 0; bm < BPB || bm + b < spb.size; bm++) {
            // actually we iterating through "bits in byte"
            m = 1 << (bm % 8);
            // is block free
            if ((buf->data[bm/8] & m) == 0) {
                // mark block as not free
                buf->data[bm/8] |= m;
                bwrite(buf);
                brelease(buf);
                zeroblock(b + bm);
                return b + bm; // block number that we allocate
            }
        }
        brelease(buf);
    }
    printf("ERROR: out of disk block\n");
}

/*
 * mark block as free in corresponding bitmap by block number
 */
static void
bfree(int blockn) {
    struct CacheBuffer *b;
    // bitmap block
    b = bread((BMBLOCK(block_num, spb)));

    int bm = block_num % BPB;
    int m = 1 << (bm % 8);

    if (b->data[bm/8] & m == 0)
        printf("ERROR: try to free block, that already free");

    // 000-1-000 -> 000-0-000
    b->data[bm/8] &= ~m;
    bwrite(b);
    brelease(b);
}

// Inodes code
//
// ialloc(int dev, int num) -- allocate fresh inode in disk
//                             (create file actually)
// iget(int dev, int num) -- return in memory copy of demanding inode
//                           if no such inode -> process empty slot
// ilock(struct inode *inode) -- locking in memory inode, so you can modify it
//                               read from memory if necessary

// In memory inodes
struct inode inodes[INODES_NUM];

/*
 * allocating new inode on disk
 */
struct inode*
ialloc(int type)
{
    struct CacheBuffer *b;
    struct dinode *din;

    for (int i = 0; i < spb.inodes_num; i++) {
        int ib = IBLOCK(i, spb);
        b = bread(ib);
        din = (struct dinode*)b->data + i % INODES_PER_BLOCK;
        if (din->type == FREE) {
            memset(din, 0, sizeof(struct dinode));
            din->type = type;
            bwrite(b);
            brelease(b);
            return iget(dev, i);
        }
        brelease(b);
    }
    printf("ERROR: no free inodes\n");
}

/*
 * copy in memory inode to disk
 */
void
iupdate(struct inode *in)
{
    struct CacheBuffer *b;
    struct dinode *dn;

    b = bget(in->dev, IBLOCK(in->inum, spb));
    dn = (struct dinode*)b->data + in->inum % INODES_PER_BLOCK;
    dn->type = in->type;
    dn->nlink = in->nlink;
    dn->size = in->size;
    memmove(dn->addrs, in->addrs, sizeof(in->addrs));
    bwrite(b);
    brelease(b);
}


/*
 * return in memory inode
 * if not find => return new one
 * doesnt read from disk => valid = 0
 */
struct inode*
iget(int inum)
{
    struct inode *in, *empty;

    empty = NULL;

    for (in = inodes; in < &inodes[INODES_NUM]; in++) {
        if (in->ref > 0 && in->inum == inum) {
            in->ref++;
            return in;
        }
        if (in->ref == 0 && empty == NULL)
            empty = in;
    }
    if (empty == NULL)
        printf("ERROR: don't finding inode in memory\n");
    in = empty;
    in->inum = inum;
    in->ref = 1;
    in->valid = 0;
    return in;
}

/*
 * truncate inode on disk
 */
void
itrunc(struct inode *ip)
{
    for (int i = 0; i < DATA_BLOCKS_NUM; i++) {
        if (ip->addrs[i]) {
            bfree(ip->dev, i);
            ip->addrs[i] = 0;
        }
    }
}

/* drop reference on inode in memory
 * if that was last memory reference, recycle inode vacation in memory
 * if no more dir links to inode on disk, free inode in memory and on disk
 */
void
iput(struct inode *ip)
{
    if (ip->ref == 1 && ip->valid && ip->nlink == 0) {
        itrunc(ip);
        ip->type = 0;
        iupdate(ip);
        ip->valid = 0;
    }
    ip->ref--;
}


// inode content
// return block number in file system by 
// block number in current inode
uint32_t
bmap(struct inode *in, uint32_t bn)
{
    uint32_t addr;
    if ((addr = in->addrs[bn]) == 0)
        in->addrs[bn] = addr = balloc(in->dev);
    kprintf("Address is %d\n", addr);
    return addr;
}

// read information from inode 
// off - offset
// n   - size of information to read
uint32_t
readi(struct inode *in, uint32_t dst_addr, uint32_t off, uint32_t n)
{
    // how many bytes read
    uint32_t tot;
    // offset in current block
    uint32_t m;
    struct CacheBuffer *buf;

    if (off > in->size)
        return 0;
    if (off + n > in->size)
        n = in->size - off;

    for (tot = 0; tot < n; tot += m, off += m, dst_addr += m) {
        kprintf("Get buffer for readi\n");
        buf = bread(in->dev, bmap(in, off / BLOCK_SIZE));
        kprintf("%s\n", buf->data);

        m = min(n - tot, BLOCK_SIZE - off % BLOCK_SIZE);
        // copy data to dst addrs
        mmemmove(dst_addr, buf->data, m);
    }
}


uint32_t
writei(struct inode *in, uint32_t src_addr, uint32_t off, uint32_t n)
{
    kprintf("Size of inode %d,  %d\n", in->size, in->inum);
    uint32_t tot;
    uint32_t m;
    struct CacheBuffer *buf;

    if (off > in->size || n < 0)
        return -1;
    // impossible to read more than all filesystem have
    if (off + n > DATA_BLOCKS_NUM * BLOCK_SIZE)
        return -1;

    for (tot = 0; tot < n; tot += m, off += m, src_addr += m) {
        // iterate through inode data blocks, that inode
        // store in addrs field
        kprintf("Get buffer for writei\n");
        buf = bread(in->dev, bmap(in, off / BLOCK_SIZE));
        // first case is when we stop reading from somewhere inside block
        m = min(n - tot, BLOCK_SIZE - off % BLOCK_SIZE);
        mmemmove(buf->data, src_addr, m);
        // put buffer into disk with new data
        bwrite(buf);
        brelease(buf);
    }
    // means write more data to end of file or rewrite part of that
    in->size = (off > in->size) ? off : in->size;
    kprintf("Size of inode %d,  %d\n", in->size, in->inum);
    iupdate(in);
    return tot;
}

// as for now, we dont have directory hierarchy, because just want to test
// simple inodes with file names and one directory, that store all of them
struct inode*
dirlookup(struct inode *dir, char *filename)
{
    struct dirent *de;

    if (dir->type != DIRECTORY)
        panic("dirlookup: directory inode has wrong type");
    
    for (uint32_t off = 0; off < dir->size; off += sizeof(struct dirent)) {
        readi(dir, de, off, sizeof(struct dirent));
        if (sstrcmp(de->name, filename, MAX_FILE_NAME)) {
            return iget(0, de->inum);
        }
    }
    return 0;
}

// add new entry in directory
uint32_t
dirlink(struct inode *dir, char *name, uint32_t inum)
{
    uint32_t off;
    struct dirent *de;
    struct inode *in;

    kprintf("Size of inode %d,  %d\n", dir->size, dir->inum);
    if ((in = dirlookup(dir, name)) != 0) {
        iput(in);
        return -1;
    }
    for (off = 0; off < dir->size; off += sizeof(struct dirent)) {
        if (readi(dir, de, off, sizeof(struct dirent)) != sizeof(struct dirent))
            panic("dirlink: bad entry in directory");
        if (de->inum == 0)
            break;
    }
    mmemmove(de->name, name, 2);
    de->inum = inum;
    kprintf("Size of inode %d,  %d\n", dir->size, dir->inum);
    if (writei(dir, de, off, sizeof(struct dirent)) != sizeof(struct dirent))
        panic("dirlink: can't write new entry to dir");

    return 0;
}
