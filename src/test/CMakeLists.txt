add_executable(test
    test.c
    test_common.c
    test_lexer.c
)
target_include_directories(test
    PRIVATE ${CMAKE_SOURCE_DIR}/mastack
)
target_link_libraries(test
    PRIVATE greatest
            lexer
)