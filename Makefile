CFLAGS= -Wall -Wextra -std=c11 -pedantic -ggdb

heap: custom_allocator.c
	mkdir -p build
	$(CC) $(CFLAGS) -o build/heap custom_allocator.c