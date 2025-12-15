#include<assert.h>
#include<stddef.h>
#include<stdbool.h>
#include<stdio.h>

#define HEAP_CAP 640000 // Total Heap Capacity
#define HEAP_ALLOCED_CAP 1024 // Memory Chunks Capacity


//  Metadata Table
typedef struct {
    void* start;
    size_t size;
} Heap_Chunk;


char heap[HEAP_CAP]; //Memory
size_t heap_size = 0; // Used heap size

Heap_Chunk heap_alloced[HEAP_ALLOCED_CAP] = {0}; //Metadata Chunks
size_t heap_alloced_size = 0; //Used Heap Metadata Chunk



//Heap Chunks
void *heap_alloc(size_t size){
    assert(heap_size + size <= HEAP_CAP); // check if there is enough memory
    void* result = heap + heap_size; // free space pointer
    heap_size += size; //moving pointer to next free pointer
    
    //Metadata of data stored
    const Heap_Chunk chunk = {
        .start = result,
        .size = size,
    };

    assert(heap_alloced_size <= HEAP_ALLOCED_CAP);
    //Adding Metadata to array
    heap_alloced[heap_alloced_size++] = chunk;

    return result;
}

void heap_dump_alloced_chunks(void){
    printf("Allocated Chunks: %zu\n", heap_alloced_size);
    for(size_t i = 0; i < heap_alloced_size; ++i){
        printf(
            "   start: %p, size: %zu\n",
            heap_alloced[i].start,
            heap_alloced[i].size            
        );            
    }
}

void heap_free(void* ptr){
    (void) ptr;
}

void heap_collect(){
    assert(false && "Not Implemented");
}

int main(int argc, char* argv[]){
    char* root = heap_alloc(26);
    
    for(int i = 0; i < 26; ++i){
        root[i] = i + 'A';
    }

    heap_dump_alloced_chunks();
    
    return 0;
}