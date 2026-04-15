#include "greatest.h"
#include "util/buf_writer.h"
#include "lexer/token.h"
#include "lexer/lexer.h"

// Normal line
#define LINE_NM(text)   \
    text

#define EOL "\n"

// EOL-terminated line
#define LINE_ET(text)   \
    text EOL

// Assert line info
#define ASSERT_LI(lc, row, off_, len_, eol_)                    \
    do {                                                        \
        LineInfo * info = LineCache_at(lc, row);                \
        ASSERT_EQ_FMT((usize)off_, (usize)info->off, "%zu");    \
        ASSERT_EQ_FMT((usize)len_, (usize)info->len, "%zu");    \
        ASSERT_EQ_FMT(eol_, (Eol)info->eol, "%d");              \
        off_ += (usize)len_ + Eol_length((Eol)info->eol);       \
    } while (0)

TEST test_tokenization_all_kinds_of_token(void) {
    BufSlice text = BufSlice_new_from_str(
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

    LineCache lc;
    TokSeq stm;
    LexRpt rpt;
    Lexer_extract(&lex, &lc, &stm, &rpt);

    usize off = 0;

    ASSERT_LI(&lc, 0, off, 33, Eol_Lf);

    ASSERT_LI(&lc, 1, off, 41, Eol_Lf);
    ASSERT_LI(&lc, 2, off, 29, Eol_Lf);
    ASSERT_LI(&lc, 3, off, 1, Eol_Lf);
    ASSERT_LI(&lc, 4, off, 0, Eol_Lf);
    ASSERT_LI(&lc, 5, off, 64, Eol_Lf);
    ASSERT_LI(&lc, 6, off, 15, Eol_Lf);
    ASSERT_LI(&lc, 7, off, 19, Eol_Lf);
    ASSERT_LI(&lc, 8, off, 22, Eol_Lf);
    ASSERT_LI(&lc, 9, off, 20, Eol_Lf);
    ASSERT_LI(&lc, 10, off, 12, Eol_Lf);
    ASSERT_LI(&lc, 11, off, 19, Eol_Lf);
    ASSERT_LI(&lc, 12, off, 5, Eol_Lf);
    ASSERT_LI(&lc, 13, off, 1, Eol_None);

    ASSERT_EQ(0, LexRpt_count(&rpt));

    LexRpt_deinit(&rpt);
    TokSeq_deinit(&stm);
    LineCache_deinit(&lc);

    Lexer_deinit(&lex);

    PASS();
}

SUITE(suite_lexer) {
    RUN_TEST(test_tokenization_all_kinds_of_token);
}