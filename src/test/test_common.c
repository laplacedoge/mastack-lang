#include "greatest.h"
#include "util/buf_writer.h"
#include "lexer/token.h"

TEST Test_demo(void) {
    BufWriter wrt;
    BufWriter_init(&wrt, 1);

    TokSeq seq;
    TokSeq_init(&seq);

    ASSERT(TokSeq_push_single_line_comment(&seq,
        BufSlice_new_from_str(" A simple \"assignment statement\"")));
    ASSERT(TokSeq_push_tagonly(&seq, TokTag_Let));
    ASSERT(TokSeq_push_name(&seq, BufSlice_new_from_str("age")));
    ASSERT(TokSeq_push_tagonly(&seq, TokTag_Colon));
    ASSERT(TokSeq_push_name(&seq, BufSlice_new_from_str("Integer")));
    ASSERT(TokSeq_push_tagonly(&seq, TokTag_Assign));
    ASSERT(TokSeq_push_integer(&seq, 47));
    ASSERT(TokSeq_push_tagonly(&seq, TokTag_Semicolon));

    usize num_toks = TokSeq_count(&seq);
    ASSERT_EQ(num_toks, 8);

    ASSERT(TokSeq_write(&seq, &wrt));
    ASSERT(BufWriter_write_str(&wrt, "\r\n"));
    ASSERT(BufWriter_flush(&wrt));

    TokSeq_deinit(&seq);

    BufWriter_deinit(&wrt);

    PASS();
}

SUITE(Suite_common) {
    RUN_TEST(Test_demo);
}