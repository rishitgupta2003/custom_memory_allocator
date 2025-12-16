CFLAGS= -Wall -Wextra -std=c11 -pedantic -ggdb

custom: custom_allocator.c
	mkdir -p build
	$(CC) $(CFLAGS) -o build/heap custom_allocator.c

clean:
	rm -rf build

#for final build
#not needed at the moment
final:	final_allocator.c 
	mkdir -p final_build
	$(CC) $(CFLAGS) -o final_build/final_allocator final_allocator.c