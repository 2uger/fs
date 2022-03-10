#ifndef DISK_EMULATOR_H
#define DISK_EMULATOR_H

#include <stdint.h>
#include "param.h"

int read_block(int, int, void*);
int write_block(int, void*, int);

struct block {
    char data[BLOCK_SIZE];        
};

#endif
