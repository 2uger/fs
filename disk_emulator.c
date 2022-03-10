// Simple emulator of hard drive

#include <stdint.h>
#include <stdio.h>
#include <assert.h>


#define BLOCK_SIZE 512
#define BLOCKS_NUM 512


struct block {
    char data[BLOCK_SIZE];        
};

struct block DISK[BLOCKS_NUM];


int read_block(int, int, void*);
int write_disk(int, void*, int);


/*
 * blockn - block number(start with zero)
 * blocks_num - how much blocks to read
 * buffer - where to write
 */
int 
read_block(int blockn, int blocks_num, void *buffer)
{
    if (blockn < 0 || blockn >= BLOCKS_NUM)
        return 0;

    // how many bytes we actually read from disk
    int byte_counter = 0;

    struct block *b = DISK + blockn;
    char *buf = buffer;

    while (blocks_num) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            *buf = b->data[i]; 
            buf++;
            byte_counter++;
        }
        blocks_num--;
        b++;

        // avoid reading from DISK bound
        if (b >= &DISK[BLOCKS_NUM])
            break;
    }
    return byte_counter;
} 

/*
 * block - block number(start with zero)
 * buffer - where to read
 * size - data size in bytes
 */
int
write_block(int blockn, void *buffer, int size)
{
    uint8_t remainder = (size % BLOCK_SIZE) > 0 ? 1 : 0;
    int blocks_num = (int)(size / BLOCK_SIZE) + remainder;

    if (blockn + blocks_num > BLOCKS_NUM) {
        printf("Out of blocks!\n");
        return 0;
    }

    int byte_counter = 0;

    char *buf = buffer;

    struct block *b;
    b = DISK + blockn;

    while (size) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            b->data[i] = *buf;
            buf++;
            byte_counter++;
            size--;
            
            if (size == 0) {
                break;
            }
        }
        b++;
    }
    return byte_counter; 
}

int
main()
{
    char block_storage[BLOCK_SIZE];
    char test_string[] = "Hello world!";

    int bytes = 0;
    bytes = write_block(0, test_string, sizeof(test_string));
    assert(bytes == sizeof(test_string));

    bytes = read_block(0, 1, block_storage);
    assert(bytes == BLOCK_SIZE);

    bytes = write_block(0, test_string, BLOCKS_NUM * BLOCK_SIZE + 1);
    assert(bytes == 0);
    printf("Tests passed!!!\n");
    return 0;
}

