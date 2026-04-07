#ifndef __MASTACK_LEXER_ERROR_H__
#define __MASTACK_LEXER_ERROR_H__

#include "common/common.h"
#include "util/mut_buf.h"

/**
 * @brief Lexer error kind.
 */
typedef enum _LexErrKind {
    LexErrKind_InvalidByte,
} LexErrKind;

const char *
LexErrKind_to_cstr(
    LexErrKind kind
);

/**
 * @brief Lexer error.
 */
typedef struct _LexErr {
    LexErrKind kind;
    union {
        struct {
            usize row;
            usize col;
            u8 byte;
        } inv_byte;
    } v;
} LexErr;

void
LexErr_init_invalid_byte(
    LexErr * self,
    usize row,
    usize col,
    u8 byte
);

void
LexErr_deinit(
    LexErr * self
);

/**
 * @brief Lexer report.
 */
typedef struct _LexRpt {
    MutBuf buf;
    usize cnt;
} LexRpt;

void
LexRpt_init(
    LexRpt * self
);

bool
LexRpt_add(
    LexRpt * self,
    LexErr * err
);

bool
LexRpt_add_error_invalid_byte(
    LexRpt * self,
    usize row,
    usize col,
    u8 byte
);

static
inline
usize
LexRpt_count(
    LexRpt * self
) {
    return self->cnt;
}

static
inline
LexErr *
LexRpt_at(
    LexRpt * self,
    usize idx
) {
    return &MutBuf_at_as(&self->buf, LexErr, idx);
}

#define LexRpt_for_each(self, name, code)               \
    for (usize i = 0; i < LexRpt_count(self); i++) {    \
        LexErr * name = LexRpt_at(self, i);             \
        code                                            \
    }

void
LexRpt_clear(
    LexRpt * self
);

void
LexRpt_deinit(
    LexRpt * self
);

#endif