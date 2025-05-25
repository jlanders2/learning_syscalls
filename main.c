#include "memory.h"

int main() {
  int *p = my_malloc(sizeof(int *));
  debug_heap();
  return 0;
}
