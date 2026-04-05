add_library(lexer STATIC token.c token.h)
target_link_libraries(lexer
    PRIVATE memory
    PUBLIC common mut_buf buf_writer
)