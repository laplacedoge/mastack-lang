#ifndef __MASTACK_LEXER_LEXER_H__
#define __MASTACK_LEXER_LEXER_H__

#include "common/common.h"
#include "lexer/token.h"
#include "lexer/line.h"

typedef enum _Eol {
    Eol_None,
    Eol_Cr,
    Eol_Lf,
    Eol_CrLf,
} Eol;

typedef enum _LexRes {
    LexErrKind_NoMem,
} LexErrKind;

typedef struct _LexErr {
    LexErrKind kind;
    union {
        struct {

        } no_mem;
    } v;
} LexErr;

typedef struct _Lexer {
    u32 stat:8;
    u32 should_mark_newline:1;

    LineCache lc;   // Line cache
    MutBuf str;
    TokSeq stm;

    usize cur_idx;  // Current index
    usize nl_idx;   // New line index

    LexErr err;
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
    TokSeq * seq
);

void
Lexer_deinit(
    Lexer * self
);

#endif