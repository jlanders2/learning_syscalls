#include "memory.h"
#include <stdio.h>
int main() {
  debug_heap();
  int *p = my_malloc(sizeof(int));
  int *p2 = my_malloc(sizeof(int));
  int *p3 = my_malloc(sizeof(int));
  int *p4 = my_malloc(sizeof(int));
  int *p5 = my_malloc(sizeof(int));
  int *p6 = my_malloc(sizeof(int));
  my_free(p);
  my_free(p6);
  my_free(p3);
  debug_heap();
  my_free(p2);
  debug_heap();
  my_free(p4);
  my_free(p5);
  debug_heap();
  return 0;
}
