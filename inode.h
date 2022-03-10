#ifndef FS_H
#define FS_H

#include <stdint.h>

#include "param.h"


#define INODES_PER_BLOCK (BLOCK_SIZE / sizeof(struct dinode))

// disk block containing inode
#define IBLOCK(i, sb) ((i/INODES_PER_BLOCK) + sb.inodes_start)

// bit per block
#define BPB (BLOCK_SIZE * 8) // as we measure block size in bytes

// disk block containing bitmap bit for demanding block
#define BMBLOCK(b, spb) ((b/BPB) + spb.bitmap_start)

#define min(a, b) (a < b ? a : b)


// superblock
struct spblock {
    int magic; // FS magic number, used to recognize file system
    int size;  // FS size, measured in blocks amount

    int data_blocks_num;
    int inodes_num;

    int inodes_start; // block where inodes start
    int bitmap_start; // block where bitmap start
}__attribute__((packed));

// disk inode structure
struct dinode {
    int type;  // identify type of information(file, dir, device). 0 means free.
    int nlink; // how much dirs link to that file
    int size;  // how much data in file
    int addrs[DATA_BLOCKS_NUM]; // addresses of all file's blocks
}__attribute__((packed));

// memory inode structre
struct inode {
    int inum;  // inode number
    int ref;   // count all reference from C code(pointers), remove when 0
    int valid; // usage: when find free place in inode table, but dont read from disk

    // same structure as on disk inode
    int type; 
    int nlink;
    int size;
    int addrs[DATA_BLOCKS_NUM];
}__attribute__((packed));

// directory is a file containing links to file
struct dirent {
    char name[MAX_FILE_NAME];
    int inum;
}__attribute__((packed));

enum InodeType {FREE=0, FILE, DIRECTORY, DEVICE};

#endif
