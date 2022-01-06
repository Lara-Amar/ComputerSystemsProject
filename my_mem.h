#ifndef MY_MEM_H
#define MY_MEM_H

typedef struct {
    int num_blocks_used;
    int num_blocks_free;
    int smallest_block_free;
    int smallest_block_used;
    int largest_block_free;
    int largest_block_used;
} mem_stats_struct, *mem_stats_ptr;

void mem_init(unsigned char *my_memory, unsigned int my_mem_size);

void *my_malloc(unsigned size);
void my_free(void *mem_pointer);

void mem_get_stats(mem_stats_ptr mem_stats_ptr);

#endif
