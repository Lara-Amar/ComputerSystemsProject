testing:
	gcc -std=c99 -g -pedantic-errors -Wunused testing.c my_mem.c -o test

provided:
	gcc -std=c99 -g -pedantic-errors -Wunused provided.c my_mem.c -o provided
