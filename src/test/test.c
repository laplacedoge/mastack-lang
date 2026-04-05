#include "greatest.h"

SUITE(Suite_common);

GREATEST_MAIN_DEFS();

int
main(
    int argc,
    char ** argv
) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(Suite_common);

    GREATEST_MAIN_END();
}