#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "my_mem.h"

struct timeval last_time;

void update_time(void)
{
    gettimeofday(&last_time, NULL);
}

void print_time(void)
{
    struct timeval now, diff;

    update_time();
    gettimeofday(&now, NULL);

    diff.tv_sec = now.tv_sec - last_time.tv_sec;
    diff.tv_usec = now.tv_usec - last_time.tv_usec;

    printf("test took %ld.%06ld seconds.\n", diff.tv_sec, diff.tv_usec);

    update_time();
}

void run_test(bool success, char *message)
{
    printf("running test \"%s\"...\n", message);

    if (success)
    {
        printf("success!\n");
    }
    else
    {
        printf("failed.\n");
    }

    print_time();
    printf("\n");
}

// allocate random amounts a bunch of times
bool random(void) 
{
    mem_stats_struct stats;

    for (int i = 0; i < 10000; i++)
    {
        void *ptr = my_malloc(rand() % 20000 + 1);
        mem_get_stats(&stats);

        if (ptr == NULL || stats.num_blocks_used != 1)
        {
            return false;
        }

        my_free(ptr);
        mem_get_stats(&stats);

        if (stats.num_blocks_used != 0)
        {
            return false;
        }
    }

    mem_get_stats(&stats);

    return stats.num_blocks_used == 0 && stats.num_blocks_free == 1;
}

// allocate 1 bytes a bunch of times and then free them
bool tiny_allocs(void)
{
    mem_stats_struct stats;

    int num_allocs = 1000;
    void *pointers[num_allocs];

    for (int i = 0; i < num_allocs; i++)
    {
        void *ptr = my_malloc(1);
        mem_get_stats(&stats);

        if (ptr == NULL || stats.num_blocks_used != i + 1)
        {
            return false;
        }

        pointers[i] = ptr;
    }

    for (int i = 0; i < num_allocs; i++)
    {
        my_free(pointers[i]);
        mem_get_stats(&stats);

        if (stats.num_blocks_used != num_allocs - i - 1)
        {
            return false;
        }
    }

    mem_get_stats(&stats);

    return stats.num_blocks_used == 0 && stats.num_blocks_free == 1;
}

// try out the edge cases
bool edge_cases(void)
{
    void *ptr = my_malloc(0);

    if (ptr == NULL)
    {
        return false;
    }

    my_free(ptr);

    ptr = my_malloc(1024 * 1024 * 1024);

    if (ptr != NULL)
    {
        return false;
    }

    my_free(ptr);
    my_free(NULL);

    return true;
}

int main()
{
    unsigned int mem_size = 1024 * 1024;
    unsigned char *memory = malloc(mem_size);

    update_time();

    mem_init(memory, mem_size);
    run_test(random(), "random allocations");

    mem_init(memory, mem_size);
    run_test(tiny_allocs(), "tiny allocations");

    mem_init(memory, mem_size);
    run_test(edge_cases(), "edge cases");

    free(memory);

    return 0;
}
