#include "memory.h"

#include <math.h>
#include <stdio.h>

#define HEAP_SIZE 4096
#define BYTE_CONVERSION 1024

typedef struct block {
    size_t size;
    struct block *next;
    int free;
} block_t;

static char heap[HEAP_SIZE];
static int heap_initialized = 0;

void init_heap() {
    block_t *initial = (block_t *)heap;
    initial->size = HEAP_SIZE - sizeof(block_t);
    initial->next = NULL;
    initial->free = 1;
    heap_initialized = 1;
}

void *my_malloc(size_t size) {
    if (!heap_initialized) init_heap();
    block_t *current = (block_t *)heap;
    while (current) {
        if (current->size >= size && current->free) {
            break;
        }
        current = current->next;
    }
    block_t *next = (block_t *)((char *)current + size + sizeof(block_t));
    next->size = current->size - size - sizeof(block_t);
    next->free = 1;
    next->next = NULL;
    current->size = size;
    current->next = next;
    current->free = 0;
    return (char *)current + sizeof(block_t);
}

void debug_heap() {
    if (!heap_initialized) init_heap();
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
            printf("[%luMB%s]\n",
                   current->size / (size_t)pow(BYTE_CONVERSION, 2), freeStr);
        current = current->next;
    }
}
