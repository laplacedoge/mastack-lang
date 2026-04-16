#include "greatest.h"
#include "lexer/token.h"
#include "lexer/lexer.h"

// Normal line
#define LINE_NM(text)   \
    text

#define EOL "\n"

// EOL-terminated line
#define LINE_ET(text)   \
    text EOL

#define TAGONLY(tag)                    \
    ({                                  \
        Token tok;                      \
        Token_init_tagonly(&tok, tag);  \
        tok;                            \
    })

#define NAME(name)                                      \
    ({                                                  \
        Token tok;                                      \
        ASSERT(Token_init_name_from_cstr(&tok, name));  \
        tok;                                            \
    })

#define INTEGER(val)                    \
    ({                                  \
        Token tok;                      \
        Token_init_integer(&tok, val);  \
        tok;                            \
    })

#define SL_COMMENT(comment)                                     \
    ({                                                          \
        Token tok;                                              \
        ASSERT(Token_init_sl_comment_from_cstr(&tok, comment)); \
        tok;                                                    \
    })

#define PRE_ASSERT_LINES()  \
    usize byte_off = 0;     \
    usize tok_off = 0;      \

#define ASSERT_LINE(lex_, row_, len_, eol_, ...)                    \
    do {                                                            \
        LineInfo * info = LineCache_at(&lex_.lc, row_);             \
        ASSERT_EQ_FMT((usize)byte_off, (usize)info->off, "%zu");    \
        ASSERT_EQ_FMT((usize)len_, (usize)info->len, "%zu");        \
        ASSERT_EQ_FMT(eol_, (Eol)info->eol, "%d");                  \
        byte_off += (usize)len_ + Eol_length((Eol)info->eol);       \
        Token toks[] = __VA_ARGS__;                                 \
        for (usize i = 0; i < ARRAY_SIZE(toks); i++) {              \
            if (!Token_compare(&toks[i],                            \
                    TokSeq_at(&lex.stm, tok_off + i))) {            \
                FAIL();                                             \
            }                                                       \
            Token_deinit(&toks[i]);                                 \
        }                                                           \
        tok_off += ARRAY_SIZE(toks);                                \
    } while (false);

#define POST_ASSERT_LINES() \

TEST test_tokenization_all_kinds_of_token(void) {
    BufSlice text = BufSlice_new_from_cstr(
        LINE_ET("// This is a single-line comment!")
        LINE_ET("fn calculation_1(x: Integer) -> Integer {")
        LINE_ET("    return 100 / (10 - x) * x")
        LINE_ET("}")
        LINE_ET("")
        LINE_ET("fn comparison_1(a: Boolean, b: Boolean, c: Boolean) -> Boolean {")
        LINE_ET("    if  a | b {")
        LINE_ET("        return true")
        LINE_ET("    } else if !b & c {")
        LINE_ET("        return false")
        LINE_ET("    } else {")
        LINE_ET("        return true")
        LINE_ET("    }")
        LINE_NM("}")
    );

    Lexer lex;
    Lexer_init(&lex);

    ASSERT(Lexer_tokenize(&lex, text));

    ASSERT_EQ(0, LexRpt_count(&lex.rpt));

    PRE_ASSERT_LINES();

    ASSERT_LINE(lex, 0, 33, Eol_Lf, {
        SL_COMMENT(" This is a single-line comment!"),
    });
    ASSERT_LINE(lex, 1, 41, Eol_Lf, {
        TAGONLY(TokTag_Fn),
        NAME("calculation_1"),
        TAGONLY(TokTag_LeftParen),
        NAME("x"),
        TAGONLY(TokTag_Colon),
        NAME("Integer"),
        TAGONLY(TokTag_RightParen),
        TAGONLY(TokTag_RightArrow),
        NAME("Integer"),
        TAGONLY(TokTag_LeftBrace),
    });
    ASSERT_LINE(lex, 2, 29, Eol_Lf, {
        TAGONLY(TokTag_Return),
        INTEGER(100),
        TAGONLY(TokTag_ForwardSlash),
        TAGONLY(TokTag_LeftParen),
        INTEGER(10),
        TAGONLY(TokTag_Hyphen),
        NAME("x"),
        TAGONLY(TokTag_RightParen),
        TAGONLY(TokTag_Asterisk),
        NAME("x"),
    });
    ASSERT_LINE(lex, 3, 1, Eol_Lf, {
        TAGONLY(TokTag_RightBrace),
    });
    ASSERT_LINE(lex, 4, 0, Eol_Lf, {});
    ASSERT_LINE(lex, 5, 64, Eol_Lf, {
        TAGONLY(TokTag_Fn),
        NAME("comparison_1"),
        TAGONLY(TokTag_LeftParen),
        NAME("a"),
        TAGONLY(TokTag_Colon),
        NAME("Boolean"),
        TAGONLY(TokTag_Comma),
        NAME("b"),
        TAGONLY(TokTag_Colon),
        NAME("Boolean"),
        TAGONLY(TokTag_Comma),
        NAME("c"),
        TAGONLY(TokTag_Colon),
        NAME("Boolean"),
        TAGONLY(TokTag_RightParen),
        TAGONLY(TokTag_RightArrow),
        NAME("Boolean"),
        TAGONLY(TokTag_LeftBrace),
    });
    ASSERT_LINE(lex, 6, 15, Eol_Lf, {
        TAGONLY(TokTag_If),
        NAME("a"),
        TAGONLY(TokTag_Or),
        NAME("b"),
        TAGONLY(TokTag_LeftBrace),
    });
    ASSERT_LINE(lex, 7, 19, Eol_Lf, {
        TAGONLY(TokTag_Return),
        TAGONLY(TokTag_True),
    });
    ASSERT_LINE(lex, 8, 22, Eol_Lf, {
        TAGONLY(TokTag_RightBrace),
        TAGONLY(TokTag_Else),
        TAGONLY(TokTag_If),
        TAGONLY(TokTag_Not),
        NAME("b"),
        TAGONLY(TokTag_And),
        NAME("c"),
        TAGONLY(TokTag_LeftBrace),
    });
    ASSERT_LINE(lex, 9, 20, Eol_Lf, {
        TAGONLY(TokTag_Return),
        TAGONLY(TokTag_False),
    });
    ASSERT_LINE(lex, 10, 12, Eol_Lf, {
        TAGONLY(TokTag_RightBrace),
        TAGONLY(TokTag_Else),
        TAGONLY(TokTag_LeftBrace),
    });
    ASSERT_LINE(lex, 11, 19, Eol_Lf, {
        TAGONLY(TokTag_Return),
        TAGONLY(TokTag_True),
    });
    ASSERT_LINE(lex, 12, 5, Eol_Lf, {
        TAGONLY(TokTag_RightBrace),
    });
    ASSERT_LINE(lex, 13, 1, Eol_None, {
        TAGONLY(TokTag_RightBrace),
    });

    POST_ASSERT_LINES();

    Lexer_deinit(&lex);

    PASS();
}

SUITE(suite_lexer) {
    RUN_TEST(test_tokenization_all_kinds_of_token);
}