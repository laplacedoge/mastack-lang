#include <stdlib.h>

#include "memory.h"

void *
Mem_malloc(
    usize size
) {
    return malloc(size);
}

void *
Mem_realloc(
    void * ptr,
    usize size
) {
    return realloc(ptr, size);
}

void
Mem_free(
    void * ptr
) {
    free(ptr);
}
