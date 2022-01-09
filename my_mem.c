#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "my_mem.h"

/*
 * each block of memory has a header, stored at the beginning of the block. the
 * rest of the memory in the block is for the programmer to use. headers can be
 * chained together in a linked list.
 */
typedef struct header {
    struct header *next;
    size_t size; // the size of the block (including the header)
} header_t;

typedef struct memory {
    // the heads of the linked lists of free and used blocks
    header_t *free, *used;
} memory_t;

// the global memory manager
memory_t mem;

// returns whether two headers are adjacent and can be merged
bool are_mergeable(header_t *h1, header_t *h2)
{
    int diff = (char *)h1 - (char *)h2;

    if (diff < 0)
    {
        diff = -diff;
    }

    return (h1 < h2 && diff == h1->size)
        || (h2 < h1 && diff == h2->size);
}

// merges two headers and returns the new header
header_t *merge_headers(header_t *h1, header_t *h2)
{
    if (h1 < h2)
    {
        // h1 is first
        h1->size += h2->size;
        return h1;
    }
    else
    {
        // h2 is first
        h2->size += h1->size;
        return h2;
    }
}

/*
 * take an initialized header and check if it can be merged with any other
 * free blocks. if it can, merge them. if it can't, push it on the list
 */
void push_free(header_t *header)
{
    // merge the header if possible
    header_t *other = mem.free;
    header_t *prev = NULL;

    while (other != NULL)
    {
        if (are_mergeable(header, other))
        {
            // headers are mergeable, so remove the other header from the list,
            // and push the merged header instead
            if (prev == NULL)
            {
                mem.free = other->next;
            }
            else
            {
                prev->next = other->next;
            }

            push_free(merge_headers(header, other));
            return;
        }
        prev = other;
        other = other->next;
    }

    // if the header could't be merged, just add it to the list
    header->next = mem.free;
    mem.free = header;
}

void push_used(header_t *header)
{
    header->next = mem.used;
    mem.used = header;
}

void mem_init(unsigned char *my_memory, unsigned int my_mem_size)
{
    // init mem struct
    mem.free = NULL;
    mem.used = NULL;

    // add memory
    header_t *memory = (header_t *)my_memory;

    memory->next = NULL;
    memory->size = my_mem_size;

    push_free(memory);
}

// returns NULL if out of memory
void *my_malloc(unsigned size)
{
    size_t total_size = size + sizeof(header_t);

    // look for a large enough chunk of memory
    header_t *header = mem.free;
    header_t *prev = NULL;

    while (header != NULL)
    {
        if (header->size >= total_size)
        {
            // header will fit this memory, so return a chunk of it
            if (header->size - total_size < sizeof(header_t))
            {
                // the header can't be split up, so just remove it from the free
                // list and return it
                if (prev == NULL)
                {
                    mem.free = header->next;
                }
                else
                {
                    prev->next = header->next;
                }

                push_used(header);

                return header + 1;
            }
            else
            {
                // the header can be split up, so just shrink the block and 
                // return the tail end as a new header
                header->size -= total_size;

                header_t *tail = (header_t *)((char *)header + header->size);

                tail->next = NULL;
                tail->size = total_size;

                push_used(tail);

                return tail + 1;
            }
        }
        prev = header;
        header = header->next;
    }

    // there wasn't a big enough block!
    return NULL;
}

void my_free(void *mem_pointer)
{
    // ignore null pointers
    if (mem_pointer == NULL)
        return;

    // remove the header from the used list
    header_t *header = (header_t *)mem_pointer - 1;
    header_t *other = mem.used;
    header_t *prev = NULL;

    while (other != NULL)
    {
        if (other == header)
        {
            if (prev == NULL)
            {
                mem.used = other->next;
            }
            else
            {
                prev->next = other->next;
            }

            break;
        }

        prev = other;
        other = other->next;
    }

    // add the header to the free list
    push_free(header);
}

// given the head of a list, fills in the integers with the stats
void get_list_stats(header_t *header, int *num, int *min, int *max)
{
    int num_blocks = 0;
    int min_size = 0;
    int max_size = 0;

    if (header != NULL)
    {
        size_t true_size = header->size - sizeof(header_t);

        min_size = true_size;
        max_size = true_size;

        while (header != NULL)
        {
            num_blocks++;

            if (true_size < min_size)
            {
                min_size = true_size;
            }

            if (true_size > max_size)
            {
                max_size = true_size;
            }

            header = header->next;
        }
    }

    *num = num_blocks;
    *min = min_size;
    *max = max_size;
}

void mem_get_stats(mem_stats_ptr mem_stats_ptr)
{
    get_list_stats(
        mem.free,
        &mem_stats_ptr->num_blocks_free,
        &mem_stats_ptr->smallest_block_free,
        &mem_stats_ptr->largest_block_free
    );

    get_list_stats(
        mem.used,
        &mem_stats_ptr->num_blocks_used,
        &mem_stats_ptr->smallest_block_used,
        &mem_stats_ptr->largest_block_used
    );
}
