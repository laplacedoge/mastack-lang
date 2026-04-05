#ifndef __MASTACK_MEMORY_MEMORY_H__
#define __MASTACK_MEMORY_MEMORY_H__

#include "common/common.h"

void *
Mem_malloc(
    usize size
);

void *
Mem_realloc(
    void * ptr,
    usize size
);

void
Mem_free(
    void * ptr
);

#endif