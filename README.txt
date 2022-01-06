--- Running the code ---

- Use the commands 'make provided' and then './provided' to compile and run the main function from the assignment
- Use the commands 'make testing' and then './test' to compile and run the tests

--- How does this code work? ---

My memory allocator chains memory blocks together with linked lists in order to track the free and used lists of blocks. In memory, the layout looks something like this:

+----------+ <- the header_t pointer in the free or used list points here
| header_t |
+----------+ <- my_malloc gives the programmer this address to use for whatever
|          |
|          |
|  memory  |
|          |
|          |
+----------+

After calling mem_init at the beginning of the program, the free and used lists look like this:

free: [ big huge block of memory ]
used:

As allocations are made, the program looks in the free list for a big enough block of memory. If a free block is too big, it will split it up into smaller blocks like this:

old block    new blocks

+----------+ +----------+ <- this block shrinks but stays on the free list
| header_t | | header_t |
+----------+ +----------+
|          | |          |
|          | |          |
|          | +----------+
|          | +----------+ <- this is the new used block
|          | | header_t |
|          | +----------+
|          | |          |
|          | |          |
+----------+ +----------+

After a few allocations, the free and used lists would look like this:

free: [ big huge block, but a little smaller ]
used: [ third chunk ] -> [ second chunk ] -> [ first chunk ]

(Pushing to the used list results in a reversed list)

All of the implementation of the code for the above functionality can be found by looking through my_malloc and the functions it calls.

my_free just does the opposite of my_malloc, as you would expect. When the programmer calls my_free, used chunks are moved from the used list to the free list. As blocks are added to the free list, the program looks through the free chunks to see if any chunks are adjacent in memory, and can be merged together into larger chunks. If this is true, the chunks are merged!

my_malloc and my_free try to act like the standard library malloc and free when encountering edge cases. my_malloc will return NULL when it cannot allocate memory, and when allocating zero bytes it will give a pointer to zero bytes. my_free will ignore NULL pointers, and free any other valid pointers allocated with my_malloc.

--- What is the time complexity of this code? ---

- my_malloc requires linear time to search through the free list, and constant time to allocate from a chunk. So the total time is linear, relative to the number of blocks in the free list.
- my_free requires linear time to search through the used list to remove a chunk, and exponential time to merge as many chunks together as it can in the free list recursively. So the total time is exponential, relative to the number of blocks in the free list.
