#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "inode.h"
#include "dir.h"
#include "buf.h"
#include "disk_emulator.h"
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

void
test_spb()
{
    struct CacheBuffer b;
    read_block(SPB_BLOCK_NUM, 1, b.data);

    struct spblock *disk_spb = (struct spblock*)b.data;

    assert(disk_spb->magic == MAGIC_NUM);
    assert(disk_spb->inodes_start == INODES_START);
    assert(disk_spb->size == BLOCKS_NUM);
}

void
test_create_file()
{
    char file_name[] = "file_1";
    assert(creat(file_name) == 0);
}

void
test_create_files_with_same_name()
{
    char file_name[] = "file_2";
    assert(creat(file_name) == 0);
    assert(dirlookup(root_dir, file_name) != 0);
}

void
test_write_into_file()
{
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
}

int
main()
{
    printf("Initialize file system\n");

    fs_init();

    test_create_file();
    test_create_files_with_same_name();
    test_write_into_file();
    test_spb();

    printf("All tests passed!!!\n");
    return 0;
}