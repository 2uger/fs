#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "inode.h"
#include "dir.h"
#include "calls.h"
#include "param.h"

struct inode *root_dir;

void
fs_init()
{
    init_spb();
    init_bitmap();

    // create first inode as directory to act like root /
    root_dir = ialloc(DIRECTORY);
    root_dir->type = DIRECTORY;
    root_dir->nlink = 1;
    root_dir->size = 2 * BLOCK_SIZE;

    iupdate(root_dir);
}

int
main()
{
    printf("Initialize file system\n");

    fs_init();

    char file_name[20] = "new_file";
    char file_input[100] = "File input was written by ME";
    char file_output[100];

    creat(file_name);
    write(file_name, file_input, sizeof(file_input));
    read(file_name, file_output, 0, sizeof(file_input));

    printf("Reading from file: %s\n", file_output);

    return 0;
}

