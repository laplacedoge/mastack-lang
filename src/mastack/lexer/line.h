#ifndef __MASTACK_LEXER_LINE_H__
#define __MASTACK_LEXER_LINE_H__

#include "util/mut_buf.h"
#include "util/buf_writer.h"

typedef enum _Eol {
    Eol_None,
    Eol_Cr,
    Eol_Lf,
    Eol_CrLf,
} Eol;

usize
Eol_length(
    Eol eol
);

/**
 * @brief Line info.
 */
typedef struct _LineInfo {
    u64 off:31; // The offset of the first byte of this line
    u64 len:31; // The length of this line
    u64 eol:2;
} LineInfo;

static
inline
void
LineInfo_init(
    LineInfo * self,
    usize off,
    usize len,
    Eol eol
) {
    self->off = off;
    self->len = len;
    self->eol = eol;
}

bool
LineInfo_write(
    LineInfo * self,
    BufWriter * wrt
);

void
LineInfo_deinit(
    LineInfo * self
);

/**
 * @brief Line cache, stores information of each line inside the given source text.
 */
typedef struct _LineCache {
    MutBuf buf;
    usize cnt;
} LineCache;

void
LineCache_init(
    LineCache * self
);

bool
LineCache_push(
    LineCache * self,
    LineInfo * info
);

static
inline
LineInfo *
LineCache_at(
    LineCache * self,
    usize idx
) {
    return &MutBuf_at_as(&self->buf, LineInfo, idx);
}

static
inline
usize
LineCache_count(
    LineCache * self
) {
    return self->cnt;
}

bool
LineCache_write(
    LineCache * self,
    BufWriter * wrt
);

void
LineCache_clear(
    LineCache * self
);

void
LineCache_deinit(
    LineCache * self
);

#endif