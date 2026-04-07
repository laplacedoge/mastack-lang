#ifndef __MASTACK_LEXER_LEXER_H__
#define __MASTACK_LEXER_LEXER_H__

#include "common/common.h"
#include "lexer/error.h"
#include "lexer/token.h"
#include "lexer/line.h"

typedef struct _Lexer {
    u32 stat:8;
    u32 should_mark_newline:1;

    LineCache lc;   // Line cache
    MutBuf str;
    TokSeq stm;
    LexRpt rpt;

    usize cur_idx;  // Current index
    usize nl_idx;   // New line index
    usize row;
    usize col;
} Lexer;

void
Lexer_init(
    Lexer * self
);

bool
Lexer_tokenize(
    Lexer * self,
    BufSlice text
);

void
Lexer_extract(
    Lexer * self,
    LineCache * lc,
    TokSeq * stm,
    LexRpt * rpt
);

void
Lexer_deinit(
    Lexer * self
);

#endif