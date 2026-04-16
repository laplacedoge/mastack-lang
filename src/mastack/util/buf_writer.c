#include <unistd.h>
#include <stdio.h>

#include "memory/memory.h"
#include "buf_writer.h"

void
BufWriter_init(
    BufWriter * self,
    int fd
) {
    MutBuf_init(&self->buf);
    self->fd = fd;
}

bool
BufWriter_write_all(
    BufWriter * self,
    BufSlice buf
) {
    return MutBuf_extend(&self->buf, buf);
}

bool
BufWriter_write_str(
    BufWriter * self,
    const char * str
) {
    return BufWriter_write_all(self, BufSlice_new_from_cstr(str));
}

bool
BufWriter_write_fmt(
    BufWriter * self,
    const char * fmt,
    ...
) {
    va_list args;

    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (len < 0) {
        return false;
    }

    if (len > 255) {
        bool res = false;

        usize allocated_len = len + 1;
        char * printed = (char *)Mem_malloc(allocated_len);
        if (printed == NULL) {
            return false;
        }

        va_start(args, fmt);
        vsnprintf(printed, allocated_len, fmt, args);
        va_end(args);

        BufSlice buf = BufSlice_new(printed, len);
        if (!MutBuf_extend(&self->buf, buf)) {
            goto FreeBuf;
        }

        res = true;

    FreeBuf:
        Mem_free(printed);

        return res;
    } else if (len > 0) {
        char printed[256];

        va_start(args, fmt);
        vsnprintf(printed, sizeof(printed), fmt, args);
        va_end(args);

        BufSlice buf = BufSlice_new(printed, len);
        if (!MutBuf_extend(&self->buf, buf)) {
            return false;
        }

        return true;
    } else {
        return true;
    }
}

bool
BufWriter_flush(
    BufWriter * self
) {
    const u8 * buf = MutBuf_data(&self->buf);
    usize len = MutBuf_length(&self->buf);
    if (write(self->fd, buf, len) != (ssize_t)len) {
        return false;
    }

    MutBuf_compact(&self->buf);

    return true;
}

void
BufWriter_deinit(
    BufWriter * self
) {
    MutBuf_deinit(&self->buf);
}