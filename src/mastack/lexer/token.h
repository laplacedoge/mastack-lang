#ifndef __MASTACK_LEXER_TOKEN_H__
#define __MASTACK_LEXER_TOKEN_H__

#include "util/imm_buf.h"
#include "util/buf_writer.h"

typedef enum _TokTag {
    TokTag_Name,            // Name, e.g. identifier
    TokTag_Int,             // Integer

    TokTag_Else,            // The keyword `else`
    TokTag_Fn,              // The keyword `fn`
    TokTag_If,              // The keyword `if`
    TokTag_Let,             // The keyword `let`
    TokTag_Return,          // The keyword `return`

    TokTag_False,           // Boolean literal 'false'
    TokTag_True,            // Boolean literal 'true'

    TokTag_Plus,            // Plus `+`
    TokTag_Hyphen,          // Hyphen `-`
    TokTag_Asterisk,        // Asterisk `*`
    TokTag_ForwardSlash,    // Forward slash `/`

    TokTag_Not,             // Logical Not `!`
    TokTag_And,             // Logical And `&`
    TokTag_Or,              // Logical Or `|`

    TokTag_Equal,           // Equal `==`
    TokTag_NotEqual,        // Equal `!=`
    TokTag_GreaterThan,     // Equal `>`
    TokTag_Gte,             // Equal `>=`
    TokTag_LessThan,        // Equal `<`
    TokTag_Lte,             // Equal `<=`

    TokTag_Dot,             // Dot `.`
    TokTag_RightArrow,      // Right arrow `->`

    TokTag_Assign,          // Equal `=`
    TokTag_Comma,           // Colon `,`
    TokTag_Colon,           // Colon `:`
    TokTag_Semicolon,       // Semicolon `;`

    TokTag_LeftParen,       // Left parenthesis `(`
    TokTag_RightParen,      // Right parenthesis `)`
    TokTag_LeftBracket,     // Left bracket `[`
    TokTag_RightBracket,    // Right bracket `]`
    TokTag_LeftBrace,       // Left bracket `{`
    TokTag_RightBrace,      // Right bracket `}`

    TokTag_SlComment,       // Single-line comment
} TokTag;

typedef struct _Token {

    TokTag tag;

    union {

        // For the identifier token
        struct {
            ImmBuf buf;
        } name;

        // For the integer token
        struct {
            usize val;
        } int_;

        // For the single-line comment token
        struct {
            ImmBuf buf;
        } sl_cmt;
    } v;
} Token;

void
Token_init_tagonly(
    Token * self,
    TokTag tag
);

bool
Token_init_name_from_cstr(
    Token * self,
    const char * name
);

void
Token_init_integer(
    Token * self,
    usize val
);

bool
Token_init_sl_comment_from_cstr(
    Token * self,
    const char * comment
);

bool
Token_compare(
    Token * self,
    Token * other
);

bool
Token_write(
    const Token * tok,
    BufWriter * wrt
);

void
Token_deinit(
    Token * tok
);

typedef struct _TokSeq {
    MutBuf buf;
    usize cnt;
} TokSeq;

void
TokSeq_init(
    TokSeq * self
);

bool
TokSeq_push(
    TokSeq * self,
    Token * tok
);

bool
TokSeq_push_name(
    TokSeq * self,
    BufSlice name
);

bool
TokSeq_push_integer(
    TokSeq * self,
    usize val
);

bool
TokSeq_push_single_line_comment(
    TokSeq * self,
    BufSlice comment
);

bool
TokSeq_push_tagonly(
    TokSeq * self,
    TokTag tag
);

static
inline
Token *
TokSeq_at(
    TokSeq * self,
    usize idx
) {
    return &MutBuf_at_as(&self->buf, Token, idx);
}

static
inline
usize
TokSeq_count(
    TokSeq * self
) {
    return self->cnt;
}

bool
TokSeq_write(
    TokSeq * self,
    BufWriter * wrt
);

void
TokSeq_clear(
    TokSeq * self
);

void
TokSeq_deinit(
    TokSeq * self
);

void
TokSeq_free(
    TokSeq * self
);

#endif