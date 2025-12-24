#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "final_allocator.h" 

#define UNIMPLEMENTED \
        do \
        { \
            fprintf(stderr, "%s %d: FUNC: %s is not Implemented Yet\n", \
                    __FILE__, __LINE__, __func__); \
            abort(); \
        } while (0)

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

size_t heap_size = 0;
char heap[MEM_CAPACITY] = {0};

Chunk_List allocated_chunks = {0};
Chunk_List freed_chunks = {
    .count = 1,
    .chunks = {
        [0] = {.start = heap, .size = sizeof(heap)}
    }
};

static int chunk_find(const Chunk_List* list, const void* ptr){
    for(
        size_t i = 0; i < list->count; ++i
    ){
        Chunk chunk = list->chunks[i];
        if(chunk.start == ptr){
            return (int)i;
        }
    }

    return -1;
}

static void chunk_remove(Chunk_List* list, size_t index){
    assert(index < list->count);
    for(size_t i = index; i < list->count - 1; ++i){
        list->chunks[i] = list->chunks[i+1];
    }
    list->count -=1;
}

static Chunk heap_tail(){
    return freed_chunks.chunks[freed_chunks.count-1];
}

static void chunk_insert (Chunk_List* list, const void* ptr, const size_t size){
    assert(list -> count < CHUNK_LIST_CAP);
    list->chunks[list->count].start = (char*)ptr;
    list->chunks[list->count].size = size;

    for(
        size_t i = list->count;
        i > 0 && list->chunks[i].start < list->chunks[i-1].start;
        --i
    ){
        const Chunk temp = list->chunks[i];
        list->chunks[i] = list->chunks[i-1];
        list->chunks[i-1] = temp;
    }

    list->count += 1;
}

static void chunk_end_merge(){
    if (freed_chunks.count < 2) return;

    for (size_t i = 0; i < freed_chunks.count - 1; ) {
        Chunk *current = &freed_chunks.chunks[i];
        Chunk *next    = &freed_chunks.chunks[i + 1];

        if (current->start + current->size == next->start) {
            current->size += next->size;
            chunk_remove(&freed_chunks, i + 1);
            
        } else {
            ++i;
        }
    }
}

void* heap_alloced(size_t size){
    if(size == 0) return NULL;
    assert(size < MEM_CAPACITY);
    for(size_t i = 0; i < freed_chunks.count; ++i){
        const Chunk chunk = freed_chunks.chunks[i];
        if(chunk.size >= size){
            chunk_remove(&freed_chunks, i);

            const size_t tail_size = chunk.size - size;
            chunk_insert(&allocated_chunks, chunk.start, size);

            if(tail_size > 0){
                chunk_insert(&freed_chunks, chunk.start + size, tail_size);
            }

            return chunk.start;
        }
    }

    return NULL;
}

void chunk_info(const Chunk_List* list){
    printf("\nChunks:\n");
    for(size_t i = 0; i < list->count; ++i){
        printf("\tLocation:%p\tSize:%zu\n",list->chunks[i].start, list->chunks[i].size);
    }
}

void heap_free(void* ptr){
    if(ptr == NULL) return;
    const int index = chunk_find(&allocated_chunks, ptr);
    assert(index >= 0);
    Chunk chunk = allocated_chunks.chunks[index];
    assert(chunk.start == ptr);
    chunk_insert(&freed_chunks, ptr, chunk.size);
    chunk_remove(&allocated_chunks, index);
    chunk_end_merge();
}

#define N 11
void* ptr[N];

int main(){
    return 0;
}