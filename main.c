#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

    char file_name[] = "new_file";
    char file_input_1[] = "File input was written by ME";
    char file_output[100];

    // simple write into clean file
    creat(file_name);
    write(file_name, file_input_1, sizeof(file_input_1));
    read(file_name, file_output, 0, sizeof(file_input_1));

    assert(!strcmp(file_input_1, file_output));

    char file_input_2[] = "New file input";

    // write into file with data
    write(file_name, file_input_2, sizeof(file_input_2));
    read(file_name, file_output, 0, sizeof(file_input_2));

    // check first data was not erased
    assert(!strcmp(file_input_1, file_output));

    read(file_name, file_output, sizeof(file_input_1), sizeof(file_input_2));
    // check new data was written
    assert(!(strcmp(file_input_2, file_output)));

    struct inode *file = dirlookup(root_dir, file_name);
    assert(file->size == sizeof(file_input_1) + sizeof(file_input_2));

    // return 0 if no such file exists
    assert(!dirlookup(root_dir, "random_name"));
    // such file already exists
    assert(dirlink(root_dir, file_name, 123) == -1);

    printf("All tests passed!!!\n");
    return 0;
}

