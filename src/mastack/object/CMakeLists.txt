add_library(object STATIC integer.c integer.h blob.c blob.h)
target_link_libraries(object PUBLIC common)