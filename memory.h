#ifndef MY_MEMORY__H
#define MY_MEMORY__H
#include <stdlib.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void my_free_safe(void *ptr);
void debug_heap();

#endif
