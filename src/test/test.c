#include "greatest.h"

SUITE(suite_common);
SUITE(suite_lexer);

GREATEST_MAIN_DEFS();

int
main(
    int argc,
    char ** argv
) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_common);
    RUN_SUITE(suite_lexer);

    GREATEST_MAIN_END();
}