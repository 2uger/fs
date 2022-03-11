#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "inode.h"
#include "dir.h"
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
create_file(char *file_name)
{
    struct inode *n_file = ialloc(FILEE);

    dirlink(root_dir, file_name, n_file->inum);

    char to_write[10] = "Hello!!!";
    writei(n_file, to_write, 0, sizeof(to_write));
    return 0;
}

int
read_file(char *file_name)
{
    struct inode *file = dirlookup(root_dir, file_name);

    char to_read[10];

    readi(file, to_read, 0, 10);
    printf("Reading from file: %s\n", to_read);

    return 0;
}

int
main()
{
    printf("Start initialize file system\n");

    fs_init();

    printf("You are in Vision file system, type help for commands\n");

    char cmd[10];

    while (scanf("%s", cmd)) {
        // OPEN
        if (strcmp(cmd, "OPEN") == 0) {
        } else if (strcmp(cmd, "creat") == 0) {
            char file_name[MAX_FILE_NAME];
            printf("File name:\n");
            scanf("%s", file_name);
            int res;
            res = create_file(file_name);
            if (!res)
                printf("Creating file is ok\n");
            else
                printf("Failed to create file\n");

        } else if (strcmp(cmd, "read") == 0) {
            char file_name[MAX_FILE_NAME];
            printf("File name:\n");
            scanf("%s", file_name);
            int res;
            res = read_file(file_name);
        } else if (strcmp(cmd, "WRITE") == 0) {
        } else if (strcmp(cmd, "CLOSE") == 0) {
        } else if (strcmp(cmd, "help") == 0) {

        } else {
            printf("You gotta be reading instruction well before use it!!!\n");
        }
    }
    return 0;
}

