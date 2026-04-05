# Immutable buffer
add_library(imm_buf STATIC imm_buf.c imm_buf.h)
target_link_libraries(imm_buf
    PRIVATE memory
    PUBLIC common
)

# Mutable buffer
add_library(mut_buf STATIC mut_buf.c mut_buf.h)
target_link_libraries(mut_buf
    PRIVATE memory
    PUBLIC common
)

# Buffered writer
add_library(buf_writer STATIC buf_writer.c buf_writer.h)
target_link_libraries(buf_writer
    PRIVATE memory
    PUBLIC common mut_buf
)