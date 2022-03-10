#ifndef PARAM_H
#define PARAM_H

// disk emulator
#define BLOCK_SIZE 512
#define BLOCKS_NUM 512

// cache buffers
#define BCACHE_NUM 64

// inodes

#define INODES_NUM 20
// how much data blocks inode could addressing
#define DATA_BLOCKS_NUM 16
#define MAX_FILE_NAME 16

#endif
