#ifndef __MASTACK_UTILS_MUT_BUF_H__
#define __MASTACK_UTILS_MUT_BUF_H__

#include <stdarg.h>

#include "common/common.h"

/**
 * @brief Mutable buffer.
 */
typedef struct _MutBuf {
    u8 * buf;
    usize cap;
    usize len;
} MutBuf;

void
MutBuf_init(
    MutBuf * self
);

MutBuf *
MutBuf_new(void);

MutBuf *
MutBuf_new_from_buf(
    const void * buf,
    usize len
);

MutBuf *
MutBuf_clone(
    MutBuf * self
);

bool
MutBuf_compare(
    MutBuf * self,
    MutBuf * other
);

bool
MutBuf_resize(
    MutBuf * self,
    usize len
);

void
MutBuf_clear(
    MutBuf * self
);

bool
MutBuf_push(
    MutBuf * self,
    u8 byte
);

bool
MutBuf_push_dup_byte(
    MutBuf * self,
    u8 byte,
    usize cnt
);

bool
MutBuf_extend(
    MutBuf * self,
    BufSlice slice
);

bool
MutBuf_push_buf(
    MutBuf * self,
    const void * buf,
    usize len
);

bool
MutBuf_extend_with_str(
    MutBuf * self,
    const char * str
);

bool
MutBuf_extend_with_fmt(
    MutBuf * self,
    const char * fmt,
    ...
);

bool
MutBuf_extend_with_fmt_va(
    MutBuf * self,
    const char * fmt,
    va_list args
);

void
MutBuf_left_shift(
    MutBuf * self,
    usize len
);

MutBuf *
MutBuf_new_escaped(
    MutBuf * self
);

void
MutBuf_strip(
    MutBuf * self
);

static
inline
BufSlice
MutBuf_as_slice(
    MutBuf * self
) {
    return BufSlice_new(self->buf, self->len);
}

static
inline
u8 *
MutBuf_data(
    MutBuf * self
) {
    return (u8 *)self->buf;
}

/**
 * @brief Accesses the buffer content at a specific index, cast to a type.
 * @param self  The MutBuf instance.
 * @param type  The C type to cast the data to.
 * @param index The element index (based on the size of 'type')
 */
#define MutBuf_at_as(self, type, index)  \
    (((type *)MutBuf_data(self))[(index)])

static
inline
usize
MutBuf_capacity(
    MutBuf * self
) {
    return self->cap;
}

static
inline
usize
MutBuf_length(
    MutBuf * self
) {
    return self->len;
}

bool
MutBuf_compact(
    MutBuf * self
);

bool
MutBuf_merge(
    MutBuf * dst_obj,
    MutBuf * src_obj
);

MutBuf *
MutBuf_join(
    MutBuf * self,
    MutBuf * other
);

void
MutBuf_print(
    MutBuf * self
);

void
MutBuf_deinit(
    MutBuf * self
);

void
MutBuf_free(
    MutBuf * self
);

#endif