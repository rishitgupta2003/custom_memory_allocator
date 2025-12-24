#ifndef FINAL_ALLOCATOR_H
#define FINAL_ALLOCATOR_H

#include <stddef.h>
#include <stdio.h>

#define MEM_CAPACITY 640000
#define CHUNK_LIST_CAP 1024

typedef struct {
    char *start;
    size_t size;
} Chunk;

typedef struct {
    size_t count;
    Chunk chunks[CHUNK_LIST_CAP];
} Chunk_List;

// Public functions
void* heap_alloced(size_t size);
void heap_free(void* ptr);
void chunk_info(const Chunk_List* list);

#endif // FINAL_ALLOCATOR_H