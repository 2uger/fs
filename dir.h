#ifndef DIR_H
#define DIR_H

#include <stdint.h>

#include "inode.h"

struct inode* dirlookup(struct inode*, char*);
int dirlink(struct inode*, char*, int);

#endif
