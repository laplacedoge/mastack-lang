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

typedef __uint128_t u128;
typedef __int128    s128;

typedef size_t      usize;
typedef ssize_t     ssize;

typedef struct _Range {
    u64 has_start_bound:1;
    u64 has_end_bound:1;
    u64 start:31;
    u64 end:31;
} Range;

static
inline
Range
Range_new(
    usize start,
    usize end
) {
    return (Range){
        .has_start_bound = true,
        .has_end_bound = true,
        .start = start,
        .end = end,
    };
}

static
inline
Range
Range_new_empty(void) {
    return (Range){
        .has_start_bound = true,
        .has_end_bound = true,
        .start = 0,
        .end = 0,
    };
}

static
inline
Range
Range_new_from(
    usize start
) {
    return (Range){
        .has_start_bound = true,
        .has_end_bound = false,
        .start = start,
        .end = 0,
    };
}

static
inline
Range
Range_new_to(
    usize end
) {
    return (Range){
        .has_start_bound = false,
        .has_end_bound = true,
        .start = 0,
        .end = end,
    };
}

static
inline
Range
Range_new_all(void) {
    return (Range){
        .has_start_bound = false,
        .has_end_bound = false,
        .start = 0,
        .end = 0,
    };
}

void
Range_resolve(
    Range self,
    usize len,
    usize * start,
    usize * end
);

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
BufSlice_new_from_cstr(
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

#define ARRAY_SIZE(arr) \
    (sizeof(arr) / sizeof((arr)[0]) + \
        _Generic(&(arr), typeof((arr)[0]) **: 0, default: 0))

#endif