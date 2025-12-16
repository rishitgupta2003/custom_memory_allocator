#include<assert.h>
#include<stddef.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>

#define HEAP_CAP 640000 // Total Heap Capacity
#define CHUNCK_LIST_CAP 1024 // Memory Chunks Alloced Metadata Capacity
// #define HEAP_FREED_CAP 1024 //Memory Chunk Freed Metadata Capacity

#define UNIMPLEMENTED \
        do \
        { \
            fprintf(stderr, "%s %d: FUNC: %s is not Implemented Yet\n", \
                    __FILE__, __LINE__, __func__); \
            abort(); \
        } while (0)
        

//  Metadata Table
typedef struct {
    char* start;
    size_t size;
} Chunk;

typedef struct{
    size_t count;
    Chunk chunks[CHUNCK_LIST_CAP];
} Chunk_List;



char heap[HEAP_CAP]; //Memory
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

/*
    Kind of New Way.
    Lets see if this works
*/
Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {0};

int chunk_compar(const void* a, const void* b){
    const Chunk* a_chunk = a;
    const Chunk* b_chunk = b;

    return a_chunk->start - b_chunk->start;
}

int chunk_list_find(const Chunk_List* list, void* ptr){
    // UNIMPLEMENTED;
    
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
        return (result - list->chunks) / sizeof(list->chunks[0]);
    }else return -1;

}

void chunk_list_insert(Chunk_List* list, void* start, size_t size){
    // UNIMPLEMENTED;
    assert(list->count < CHUNCK_LIST_CAP);
    
    list->chunks[list->count].start = start;
    list->chunks[list->count].size = size;

    for(size_t i = list -> count; 
            i > 0 && list->chunks[i].start < list->chunks[i-1].start;  
                --i)
        {
            const Chunk t = list->chunks[i];
            list->chunks[i] = list->chunks[i-1];
            list->chunks[i-1] = t;
    }

    list->count += 1;
}

void chunk_list_remove(Chunk_List* list, size_t index){
    // UNIMPLEMENTED;
    assert(index < list->count);
    
    for(size_t i = index; i < list->count - 1; ++i){
        list->chunks[i] = list->chunks[i+1];
    }

    list->count -= 1;
}



//Chunk View (Freed || Alloced)
void chunk_list_dump(const Chunk_List* list){
    printf("Chunks: (%zu)\n", list->count);
    for(size_t i = 0; i < list->count; ++i){
        printf(
            "   start: %p, size: %zu\n",
            list->chunks[i].start,
            list->chunks[i].size            
        );            
    }
}

//Heap Chunks
void *heap_alloc(size_t size){
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

//O(N) N -> Size of Alloced
void heap_free(void* ptr){
    // UNIMPLEMENTED;
    
    if(ptr == NULL) return;
    const int index = chunk_list_find(&alloced_chunks, ptr);
    assert(index >= 0);
    
    chunk_list_insert(
        &freed_chunks, 
        alloced_chunks.chunks[index].start,
        alloced_chunks.chunks[index].size
    );

    chunk_list_remove(
        &alloced_chunks,
        (size_t) index
    );
}

void heap_collect(){
    UNIMPLEMENTED;
}

int main(int argc, char* argv[]){
    for(int i = 0; i < 10; i++){
        void* p = heap_alloc(i);
        if(i % 2  == 0){
            heap_free(p);
        }
    }

    chunk_list_dump(&alloced_chunks);
    chunk_list_dump(&freed_chunks);

    return 0;
}