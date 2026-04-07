#include "greatest.h"

SUITE(Suite_common);
SUITE(Suite_lexer);

GREATEST_MAIN_DEFS();

int
main(
    int argc,
    char ** argv
) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(Suite_common);
    RUN_SUITE(Suite_lexer);

    GREATEST_MAIN_END();
}