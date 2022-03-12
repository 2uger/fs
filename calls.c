#include <stdint.h>
#include <stddef.h>

#include "inode.h"
#include "dir.h"

extern struct inode *root_dir;

int
creat(char *file_name)
{
    struct inode *n_file = ialloc(FILEE);

    dirlink(root_dir, file_name, n_file->inum);

    n_file->nlink = 1;

    iupdate(n_file);
    iput(n_file);

    return 0;
}

int
write(char *file_name, const void *src, size_t count)
{
    struct inode *file = dirlookup(root_dir, file_name);

    writei(file, src, file->size, count);

    iupdate(file);
    iput(file);

    return 0;
}

int
read(char *file_name, void *dst, size_t off, size_t count)
{
    struct inode *file = dirlookup(root_dir, file_name);

    readi(file, dst, off, count);

    iput(file);

    return 0;
}

