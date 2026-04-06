#ifndef __MASTACK_LEXER_LINE_H__
#define __MASTACK_LEXER_LINE_H__

#include "util/mut_buf.h"

/**
 * @brief Line info.
 */
typedef struct _LineInfo {
    usize off;  // The offset of the first byte of this line
    usize len;  // The length of this line
} LineInfo;

static
inline
void
LineInfo_init(
    LineInfo * self,
    usize off,
    usize len
) {
    self->off = off;
    self->len = len;
}

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

void
LineCache_clear(
    LineCache * self
);

void
LineCache_deinit(
    LineCache * self
);

#endif