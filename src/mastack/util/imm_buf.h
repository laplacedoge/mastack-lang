#ifndef __MASTACK_UTILS_IMM_BUF_H__
#define __MASTACK_UTILS_IMM_BUF_H__

#include "common/common.h"

/**
 * @brief Immutable buffer.
 */
typedef struct _ImmBuf {
    u8 * buf;
    usize len;
} ImmBuf;

bool
ImmBuf_init(
    ImmBuf * self,
    usize len
);

ImmBuf *
ImmBuf_new(
    usize len
);

bool
ImmBuf_init_from_slice(
    ImmBuf * self,
    BufSlice slice
);

ImmBuf *
ImmBuf_new_from_slice(
    BufSlice slice
);

bool
ImmBuf_init_escaped_from_slice(
    ImmBuf * self,
    BufSlice slice
);

ImmBuf *
ImmBuf_new_escaped_from_slice(
    BufSlice slice
);

ImmBuf *
ImmBuf_new_from_file(
    const char * path
);

ImmBuf *
ImmBuf_new_from_file_using_imm_buf(
    ImmBuf * path
);

bool
ImmBuf_init_from_cstr(
    ImmBuf * self,
    const char * str
);

ImmBuf *
ImmBuf_new_from_str(
    const char * str
);

ImmBuf *
ImmBuf_new_from_fmt(
    const char * fmt,
    ...
);

ImmBuf *
ImmBuf_clone(
    ImmBuf * self
);

ImmBuf *
ImmBuf_new_escaped(
    ImmBuf * self
);

ImmBuf *
ImmBuf_new_null_terminated(
    ImmBuf * self
);

ImmBuf *
ImmBuf_join(
    ImmBuf * self,
    ImmBuf * other
);

bool
ImmBuf_compare(
    ImmBuf * self,
    ImmBuf * other
);

bool
ImmBuf_compare_with_str(
    ImmBuf * self,
    const char * str
);

void
ImmBuf_to_uppercase(
    ImmBuf * self
);

BufSlice
ImmBuf_as_slice(
    const ImmBuf * self
);

#define ImmBuf_rawptr_as(self, type)   \
    ((type)(self)->buf)

const u8 *
ImmBuf_data(
    ImmBuf * self
);

u8 *
ImmBuf_data_mut(
    ImmBuf * self
);

usize
ImmBuf_length(
    ImmBuf * self
);

void
ImmBuf_print(
    ImmBuf * self
);

void
ImmBuf_deinit(
    ImmBuf * self
);

void
ImmBuf_free(
    ImmBuf * self
);

#endif