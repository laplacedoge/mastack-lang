add_library(lexer
    STATIC  token.c token.h
            line.c line.h
            lexer.c lexer.h
)
target_link_libraries(lexer
    PRIVATE memory
    PUBLIC common mut_buf buf_writer
)