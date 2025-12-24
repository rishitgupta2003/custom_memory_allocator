# Simple Heap Allocator (Educational)

A small **educational memory allocator** written in C that manages a fixed-size heap using **free lists**, **allocated lists**, and **chunk merging**.

⚠️ This allocator is **not production-ready**. It is intended purely for learning, experimentation, and understanding allocator internals.

---

## Features

- Fixed-size heap (`MEM_CAPACITY`)
- First-fit allocation strategy
- Explicit chunk metadata tracking
- Free-list coalescing (merging adjacent free blocks)
- Deterministic behavior (no system `malloc`)
- Debug-friendly chunk dumping

---

## Memory Model

The allocator maintains two main chunk lists:

- `allocated_chunks` — tracks currently allocated memory blocks  
- `freed_chunks` — tracks available (free) memory blocks  

Each memory block is represented by the following structure:

```c
typedef struct {
    char *start;
    size_t size;
} Chunk;
```

---

## Public API

### `void *heap_alloced(size_t size)`
- Allocates a block of memory of the requested size.
- Uses a **first-fit strategy**.
- Splits free chunks if they are larger than requested.
- Merges adjacent free chunks before allocation.
- Returns `NULL` if allocation fails.

### `void heap_free(void *ptr)`
- Frees a previously allocated memory block.
- The pointer must be returned by `heap_alloced`.
- Double-free or invalid pointers trigger an `assert()`.

### `void chunk_info(const Chunk_List *list)`
- Prints chunk metadata for debugging purposes.

---

## Allocation Flow
1. Merge adjacent free chunks.
2. Linearly scan the free list (**first-fit**).
3. Remove the selected free chunk.
4. Split and insert remainder (if any).
5. Track allocation in `allocated_chunks`.

---

## Pitfalls & Limitations
❌ Not thread-safe (no synchronization)  
❌ No alignment guarantees (byte-aligned only)  
❌ No bounds checking (buffer overruns corrupt metadata)  
❌ Fixed metadata capacity (`CHUNK_LIST_CAP`)  
❌ Linear-time allocation and free operations (O(N))  
❌ First-fit strategy leads to fragmentation
❌ Uses `assert()` instead of graceful error handling  

---

## Notes
- Metadata can be exhausted before heap memory due to fragmentation.
- Only exact pointers returned by `heap_alloced()` may be freed.
- Best suited as an educational reference, **not a drop-in allocator**.
