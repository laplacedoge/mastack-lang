#include "greatest.h"
#include "util/buf_writer.h"
#include "lexer/token.h"
#include "lexer/lexer.h"

#define EOL "\r\n"

TEST Test_demo(void) {

    const char * TEXT =
        "fn add(a: Integer, b: Integer) -> Integer {"   EOL
        "    return a + b"                              EOL
        "}"
        ;
    BufSlice text = BufSlice_new_from_str(TEXT);

    Lexer lex;
    Lexer_init(&lex);

    ASSERT(Lexer_tokenize(&lex, text));

    LineCache lc;
    TokSeq stm;
    LexRpt rpt;
    Lexer_extract(&lex, &lc, &stm, &rpt);

    BufWriter wrt;
    BufWriter_init(&wrt, 1);

    ASSERT(TokSeq_write(&stm, &wrt));
    ASSERT(BufWriter_write_str(&wrt, "\r\n"));
    ASSERT(LineCache_write(&lc, &wrt));
    ASSERT(BufWriter_write_str(&wrt, "\r\n"));
    ASSERT(BufWriter_flush(&wrt));

    BufWriter_deinit(&wrt);

    LexRpt_deinit(&rpt);
    TokSeq_deinit(&stm);
    LineCache_deinit(&lc);

    Lexer_deinit(&lex);

    PASS();
}

SUITE(Suite_lexer) {
    RUN_TEST(Test_demo);
}