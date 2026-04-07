#ifndef __MASTACK_COMMON_COMMON_H__
#define __MASTACK_COMMON_COMMON_H__

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t     u8;
typedef int8_t      s8;

typedef uint16_t    u16;
typedef int16_t     s16;

typedef uint32_t    u32;
typedef int32_t     s32;

typedef uint64_t    u64;
typedef int64_t     s64;

typedef size_t      usize;
typedef ssize_t     ssize;

/**
 * @brief Slice of buffer.
 */
typedef struct _BufSlice {

    /* Pointer to the referenced buffer. */
    const u8 * buf;

    /* Length the referenced buffer. */
    usize len;
} BufSlice;

static
inline
BufSlice
BufSlice_new(
    const void * ptr,
    usize len
) {
    return (BufSlice){.buf = ptr, .len = len};
}

static
inline
BufSlice
BufSlice_new_empty(void) {
    return BufSlice_new(NULL, 0);
}

#define BufSlice_new_from_name(name)  \
    BufSlice_new(&name, sizeof(name))

#define BufSlice_new_from_ptr(ptr)  \
    BufSlice_new(ptr, sizeof(*ptr))

BufSlice
BufSlice_new_from_str(
    const char * str
);

usize
BufSlice_escaped_len(
    BufSlice self
);

void
BufSlice_copy_escaped(
    BufSlice self,
    u8 * buf
);

#define SHORTEN_NAME(long, short)   \
    typeof(long) * short = &long

#endif