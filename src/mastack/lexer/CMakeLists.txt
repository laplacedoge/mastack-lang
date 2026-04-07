add_library(lexer
    STATIC  error.c error.h
            lexer.c lexer.h
            line.c line.h
            token.c token.h
)
target_link_libraries(lexer
    PRIVATE memory
    PUBLIC common mut_buf buf_writer
)