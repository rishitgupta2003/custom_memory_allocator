#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define HEAP_CAP 640000 // Total Heap Capacity
#define CHUNK_LIST_CAP 1024 // Memory Chunks Alloced Metadata Capacity
// #define HEAP_FREED_CAP 1024 //Memory Chunk Freed Metadata Capacity

#define UNIMPLEMENTED \
        do \
        { \
            fprintf(stderr, "%s %d: FUNC: %s is not Implemented Yet\n", \
                    __FILE__, __LINE__, __func__); \
            abort(); \
        } while (0)

typedef struct {
    char *start;
    size_t size;
} Chunk;

typedef struct {
    size_t count;
    Chunk chunks[CHUNK_LIST_CAP];
} Chunk_List;

size_t heap_size = 0; // Used heap size

/*
    ```
    Old Way of Handling Chunks
    ```

    Chunk heap_alloced[HEAP_ALLOCED_CAP] = {0}; //Metadata Chunks
    size_t heap_alloced_size = 0; //Used Heap Metadata Chunk

    Chunk heap_freed[HEAP_FREED_CAP] = {0};
    size_t heap_freed_size = 0;
*/

char heap[HEAP_CAP] = {0};

/*
    Kind of New Way.
    Lets see if this works
*/
Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {
    .count = 1,
    .chunks = {
        [0] = {.start = heap, .size = sizeof(heap)}
    }
};
Chunk_List tmp_chunks = {0};

int chunk_compar(const void* a, const void* b){
    const Chunk* a_chunk = a;
    const Chunk* b_chunk = b;

    return (a_chunk->start < b_chunk->start) ? -1
        :  (a_chunk->start > b_chunk->start) ?  1
        :  0;
}

/*
    Deprecated One (This one would result in O(logN))
    But will not return exact index (i dunno why) 🤣🌀
*/
int chunk_list_find_deprecated(const Chunk_List* list, void* ptr){
    /*
        void *bsearch(size_t n, size_t size;
                     const void key[size], const void base[size * n],
                     size_t n, size_t size,
                     typeof(int (const void [size], const void [size]))
                         *compar);
    */

    Chunk key = {
        .start = ptr
    };

    Chunk* result = bsearch(
        &key, list->chunks, list->count, sizeof(list->chunks[0]), chunk_compar
    );

    if(result != 0){
        assert(list->chunks <= result);
        // return (int) (result - list->chunks);       
        return (int) (result - list->chunks) / sizeof(list->chunks[0]);
    } else return -1;

}


/*
    Getting it Done in O(N)
    Atleast index will be valid
*/
int chunk_list_find(const Chunk_List *list, uintptr_t *ptr){
    for (size_t i = 0; i < list->count; ++i) {
        if (list->chunks[i].start == ptr) {
            return (int) i;
        }
    }

    return -1;
}

void chunk_list_insert(Chunk_List* list, void* start, size_t size){
    assert(list->count < CHUNK_LIST_CAP);
    list->chunks[list->count].start = start;
    list->chunks[list->count].size  = size;

    for (size_t i = list->count;
            i > 0 && list->chunks[i].start < list->chunks[i - 1].start;
            --i) {
        const Chunk t = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = t;
    }

    list->count += 1;
}

void chunk_list_merge(Chunk_List *dst, const Chunk_List *src)
{
    dst->count = 0;

    for (size_t i = 0; i < src->count; ++i) {
        const Chunk chunk = src->chunks[i];

        if (dst->count == 0) {
            dst->chunks[0] = chunk;
            dst->count = 1;
            continue;
        }

        Chunk *top = &dst->chunks[dst->count - 1];

        if (top->start + top->size == chunk.start) {
            top->size += chunk.size;
        } else {
            dst->chunks[dst->count] = chunk;
            dst->count++;
        }
    }
}

//Chunk View (Freed || Alloced)
void chunk_list_dump(const Chunk_List *list)
{
    printf("Chunks (%zu):\n", list->count);
    for (size_t i = 0; i < list->count; ++i) {
        printf("  start: %p, size: %zu\n",
               list->chunks[i].start,
               list->chunks[i].size);
    }
}

void chunk_list_remove(Chunk_List *list, size_t index)
{
    assert(index < list->count);
    for (size_t i = index; i < list->count - 1; ++i) {
        list->chunks[i] = list->chunks[i + 1];
    }
    list->count -= 1;
}

//Heap Chunks
/*
    Deprecated One (So early)..... 🤣
*/
void *heap_alloc_deprecated(size_t size){
    if(size != 0){
        assert(heap_size + size <= HEAP_CAP); // check if there is enough memory
        void* ptr = heap + heap_size; // free space pointer
        heap_size += size; //moving pointer to next free pointer
        
        //Metadata of data stored
        // const Chunk chunk = {
        //     .start = result,
        //     .size = size,
        // };

        // assert(heap_alloced_size <= HEAP_ALLOCED_CAP);
        // //Adding Metadata to array
        // heap_alloced[heap_alloced_size++] = chunk;

        chunk_list_insert(&alloced_chunks, ptr, size);

        return ptr;
    }else return NULL;
}

void *heap_alloc(size_t size)
{
    if (size > 0) {
        chunk_list_merge(&tmp_chunks, &freed_chunks);
        freed_chunks = tmp_chunks;
        tmp_chunks.count = 0;

        for (size_t i = 0; i < freed_chunks.count; ++i) {
            const Chunk chunk = freed_chunks.chunks[i];
            if (chunk.size >= size) {
                chunk_list_remove(&freed_chunks, i);

                const size_t tail_size = chunk.size - size;
                chunk_list_insert(&alloced_chunks, chunk.start, size);

                if (tail_size > 0) {
                    chunk_list_insert(&freed_chunks, chunk.start + size, tail_size);
                }

                return chunk.start;
            }
        }
    }

    return NULL;
}

//O(N) N -> Size of Alloced
void heap_free(void *ptr)
{
    if (ptr != NULL) {
        const int index = chunk_list_find(&alloced_chunks, ptr);
        assert(index >= 0);
        assert(ptr == alloced_chunks.chunks[index].start);
        chunk_list_insert(&freed_chunks,
                          alloced_chunks.chunks[index].start,
                          alloced_chunks.chunks[index].size);
        chunk_list_remove(&alloced_chunks, (size_t) index);
    }
}

void heap_collect()
{
    UNIMPLEMENTED;
}

#define N 10

void *ptrs[N] = {0};

int main()
{
    for (int i = 0; i < N; ++i) {
        ptrs[i] = heap_alloc(i);
    }

    for (int i = 0; i < N; ++i) {
        if (i % 2 == 0) {
            heap_free(ptrs[i]);
        }
    }

    chunk_list_dump(&alloced_chunks);
    chunk_list_dump(&freed_chunks);

    heap_alloc(10);

    chunk_list_dump(&alloced_chunks);
    chunk_list_dump(&freed_chunks);

    return 0;
}