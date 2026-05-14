#ifndef __MASTACK_UTIL_BUF_WRITER_H__
#define __MASTACK_UTIL_BUF_WRITER_H__

#include <stdarg.h>

#include "util/mut_buf.h"

#define COLOR_RESET     "\e[0m"

#define COLOR_BLACK     "\e[30m"
#define COLOR_RED       "\e[31m"
#define COLOR_GREEN     "\e[32m"
#define COLOR_YELLOW    "\e[33m"
#define COLOR_BLUE      "\e[34m"
#define COLOR_MAGENTA   "\e[35m"
#define COLOR_CYAN      "\e[36m"
#define COLOR_WHITE     "\e[37m"

/**
 * @brief Buffered writer.
 */
typedef struct _BufWriter {
    u32 colored:1;
    MutBuf buf;
    int fd;
} BufWriter;

#define MAYBE_COLORED(self, color)  \
    (self->colored ? color : "")

void
BufWriter_init(
    BufWriter * self,
    int fd
);

static
inline
void
BufWriter_enable_color(
    BufWriter * self,
    bool colored
) {
    self->colored = colored;
}

bool
BufWriter_write_all(
    BufWriter * self,
    BufSlice buf
);

bool
BufWriter_write_cstr(
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
        BufWriter_enable_color(&wrt, true); \
        type##_write((ins), &wrt);          \
        BufWriter_write_cstr(&wrt, "\n");   \
        BufWriter_flush(&wrt);              \
        BufWriter_deinit(&wrt);             \
    } while (false)

#endif