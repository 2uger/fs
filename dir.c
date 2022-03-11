#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dir.h"
#include "param.h"
#include "inode.h"


/*
 * as for now, we dont have directory hierarchy, because just want to test
 * simple inodes with file names and one directory, that store all of them
 */
struct inode*
dirlookup(struct inode *dir, char *filename)
{
    struct dirent de;

    for (int off = 0; off < dir->size; off += sizeof(struct dirent)) {
        readi(dir, &de, off, sizeof(struct dirent));
        if (strcmp(de.name, filename) == 0)
            return iget(de.inum);
    }
    return 0;
}
struct dirent de;

/* 
 * add new entry in directory
 */
int
dirlink(struct inode *dir, char *name, int inum)
{
    int off;

    printf("1\n");
    //if ((in = dirlookup(dir, name)) != 0) {
    //    iput(in);
    //    return -1;
    //}

    printf("2\n");
    for (off = 0; off < dir->size; off += sizeof(struct dirent)) {
        printf("2\n");
        if (readi(dir, &de, off, sizeof(struct dirent)) == sizeof(struct dirent))
            printf("ERROR: dirlink: bad entry in directory\n");
        if (de.inum == 0)
            break;
        printf("readi dir\n");
    }
    printf("3\n");
    strcpy(de.name, name);
    de.inum = inum;

    printf("4\n");
    if (writei(dir, &de, off, sizeof(struct dirent)) != sizeof(struct dirent))
        printf("ERROR:dirlink: can't write new entry to dir\n");

    return 0;
}
