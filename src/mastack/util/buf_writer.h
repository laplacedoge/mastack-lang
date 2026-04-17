#ifndef __MASTACK_UTIL_BUF_WRITER_H__
#define __MASTACK_UTIL_BUF_WRITER_H__

#include <stdarg.h>

#include "util/mut_buf.h"

/**
 * @brief Buffered writer.
 */
typedef struct _BufWriter {
    MutBuf buf;
    int fd;
} BufWriter;

void
BufWriter_init(
    BufWriter * self,
    int fd
);

bool
BufWriter_write_all(
    BufWriter * self,
    BufSlice buf
);

bool
BufWriter_write_str(
    BufWriter * self,
    const char * str
);

bool
BufWriter_write_fmt(
    BufWriter * self,
    const char * fmt,
    ...
);

bool
BufWriter_flush(
    BufWriter * self
);

void
BufWriter_deinit(
    BufWriter * self
);

#define DBG(ins, type)                      \
    do {                                    \
        BufWriter wrt;                      \
        BufWriter_init(&wrt, 0);            \
        type##_write((ins), &wrt);          \
        BufWriter_write_str(&wrt, "\n");    \
        BufWriter_flush(&wrt);              \
        BufWriter_deinit(&wrt);             \
    } while (false)

#endif