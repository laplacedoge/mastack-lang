add_library(memory STATIC memory.c memory.h)
target_link_libraries(memory PUBLIC common)