#include "memory.h"

#include <math.h>
#include <stdio.h>

#define ADDRESS(x) ((unsigned long)x)

#define BYTE_CONVERSION 1024
#define HEAP_SIZE BYTE_CONVERSION * 4

typedef struct block {
  size_t size;
  int free;
  struct block *next;
  struct block *prev;
} block_t;

static char heap[HEAP_SIZE];
static int heap_initialized = 0;

void init_heap() {
  block_t *initial = (block_t *)heap;
  initial->size = HEAP_SIZE - sizeof(block_t);
  initial->next = NULL;
  initial->prev = NULL;
  initial->free = 1;
  heap_initialized = 1;
}

void *my_malloc(size_t size) {
  if (size == 0)
    return NULL;
  if (!heap_initialized)
    init_heap();
  block_t *current = (block_t *)heap;
  while (current) {
    if (current->size >= size && current->free) {
      break;
    }
    current = current->next;
  }
  if (!current) {
    printf("ERROR: Insufficient resources to allocate %lu bytes.", size);
    return NULL;
  }
  block_t *next = (block_t *)((char *)current + size + sizeof(block_t));
  next->size = current->size - size - sizeof(block_t);
  next->free = 1;
  next->next = current->next;
  next->prev = current;
  current->size = size;
  if (current->next)
    current->next->prev = next;
  current->next = next;
  current->free = 0;
  return (char *)current + sizeof(block_t);
}

void my_free(void *ptr) {
  // trust dev, kind of
  if (ptr == NULL) {
    return;
  }
  if (ADDRESS(heap) <= ADDRESS(ptr)) {
    if (ADDRESS(ptr) <= ADDRESS(heap + HEAP_SIZE - 1)) {
      block_t *meta_block = ptr - sizeof(block_t);
      if (meta_block->free)
        printf("WARN: Attempted to free unallocated pointer.\n");
      else { // Block coalescing
        meta_block->free = 1;
        if ((!meta_block->prev || !meta_block->prev->free) &&
            (!meta_block->next || !meta_block->next->free)) {
          return;
        }
        block_t *start_block = meta_block;
        while (start_block->prev && start_block->prev->free) {
          start_block = start_block->prev;
        }
        block_t *traversal_block = start_block;
        size_t new_start_size = 0;
        int freed_block_count = 0;
        while (traversal_block && traversal_block->free) {
          new_start_size += traversal_block->size;
          freed_block_count++;
          traversal_block = traversal_block->next;
        }
        start_block->size =
            new_start_size + (freed_block_count - 1) * sizeof(block_t);
        start_block->next = traversal_block;
        if (traversal_block) {
          traversal_block->prev = start_block;
        }
      }
    } else {
      printf("ERROR: Heap overflow detected for address %p\n", ptr);
    }
  } else {
    printf("ERROR: Heap underflow detected for address %p\n", ptr);
  }
}

void my_free_safe(void *ptr) {
  // do not trust dev
  // ensure ptr in heap bounds
  // start from first block and traverse until you find
  //    a block that points to the address of this ptr
  //    then update to free
  // if no block meeting criteria found, fail gracefully
}

void debug_heap() {
  printf("====BEGIN DEBUG HEAP====\n");
  if (!heap_initialized)
    init_heap();
  block_t *current = (block_t *)heap;
  while (current) {
    printf("[Metadata: %luB]\n", sizeof(block_t));
    char *freeStr = current->free ? " free" : "";

    if (current->size < BYTE_CONVERSION)
      printf("[%luB%s]\n", current->size, freeStr);
    else if (current->size >= BYTE_CONVERSION &&
             current->size < (size_t)pow(BYTE_CONVERSION, 2))
      printf("[%luKB%s]\n", current->size / BYTE_CONVERSION, freeStr);
    else if (current->size >= (size_t)pow(BYTE_CONVERSION, 2) &&
             current->size < (size_t)pow(BYTE_CONVERSION, 3))
      printf("[%luMB%s]\n", current->size / (size_t)pow(BYTE_CONVERSION, 2),
             freeStr);
    else if (current->size >= (size_t)pow(BYTE_CONVERSION, 3))
      printf("[%luGB%s]\n", current->size / (size_t)pow(BYTE_CONVERSION, 3),
             freeStr);
    current = current->next;
  }

  printf("====END  DEBUG  HEAP====\n");
}

/** TODO
- Implement double linked metadata
- Implement Free
    - Mark the block as free by setting the free flag to 1
    - Validate the pointer (ensure it's within heap bounds and properly aligned)
    - Handle invalid pointers gracefully
    - Block Coalescing
        - Check if the next block is free and merge them
        - Check if the previous block is free and merge them
        - Update size and next pointers correctly during merging
*/