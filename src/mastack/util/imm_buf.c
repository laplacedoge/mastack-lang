#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "imm_buf.h"
#include "common/common.h"
#include "memory/memory.h"

bool
ImmBuf_init(
    ImmBuf * self,
    usize len
) {
    u8 * buf;

    if (len == 0) {
        buf = NULL;
    } else {
        buf = (u8 *)Mem_malloc(len);
        if (buf == NULL) {
            return false;
        }
    }

    self->len = len;
    self->buf = buf;

    return true;
}

ImmBuf *
ImmBuf_new(
    usize len
) {
    ImmBuf * self = (ImmBuf *)Mem_malloc(sizeof(ImmBuf));
    if (self == NULL) {
        return NULL;
    }

    if (!ImmBuf_init(self, len)) {
        goto FreeSelf;
    }

    goto Exit;

FreeSelf:
    Mem_free(self);

Exit:
    return self;
}

bool
ImmBuf_init_from_slice(
    ImmBuf * self,
    BufSlice slice
) {
    u8 * buf;
    if (slice.buf == NULL || slice.len == 0) {
        buf = NULL;
    } else {
        buf = (u8 *)Mem_malloc(slice.len);
        if (buf == NULL) {
            return false;
        }

        memcpy(buf, slice.buf, slice.len);
    }

    self->buf = buf;
    self->len = slice.len;

    return true;
}

ImmBuf *
ImmBuf_new_from_slice(
    BufSlice slice
) {
    ImmBuf * self = ImmBuf_new(slice.len);
    if (self == NULL) {
        return NULL;
    }

    if (slice.len != 0) {
        memcpy(self->buf, slice.buf, slice.len);
    }

    return self;
}

bool
ImmBuf_init_escaped_from_slice(
    ImmBuf * self,
    BufSlice slice
) {
    usize escaped_len = BufSlice_escaped_len(slice);
    u8 * buf = (u8 *)Mem_malloc(escaped_len);
    if (buf == NULL) {
        return false;
    }

    printf("slice len: %zu, escaped len: %zu\r\n", slice.len, escaped_len);

    BufSlice_copy_escaped(slice, buf);
    self->buf = buf;
    self->len = escaped_len;

    return true;
}

ImmBuf *
ImmBuf_new_escaped_from_slice(
    BufSlice slice
) {
    usize escaped_len = BufSlice_escaped_len(slice);

    ImmBuf * self = ImmBuf_new(escaped_len);
    if (self == NULL) {
        goto Exit;
    }

    BufSlice_copy_escaped(slice, self->buf);

    return self;

Exit:
    return NULL;
}

ImmBuf *
ImmBuf_new_from_file(
    const char * path
) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        goto Exit;
    }

    struct stat file_stat;
    int ret = fstat(fd, &file_stat);
    if (ret != 0) {
        goto CloseFile;
    }
    usize file_size = (usize)file_stat.st_size;

    ImmBuf * self = ImmBuf_new(file_size);
    if (self == NULL) {
        goto CloseFile;
    }

    ssize_t read_len = read(fd, self->buf, file_size);
    if (read_len != (ssize_t)file_size) {
        goto FreeSelf;
    }

    close(fd);

    return self;

FreeSelf:
    ImmBuf_free(self);

CloseFile:
    close(fd);

Exit:
    return NULL;
}

ImmBuf *
ImmBuf_new_from_file_using_imm_buf(
    ImmBuf * path
) {
    ImmBuf * self = NULL;

    ImmBuf * nt_path = ImmBuf_new_null_terminated(path);
    if (nt_path == NULL) {
        goto Exit;
    }

    char * str = (char *)ImmBuf_data_mut(nt_path);
    self = ImmBuf_new_from_file(str);
    if (self == NULL) {
        goto FreeNtPath;
    }

FreeNtPath:
    ImmBuf_free(nt_path);

Exit:
    return self;
}

bool
ImmBuf_init_from_cstr(
    ImmBuf * self,
    const char * str
) {
    return ImmBuf_init_from_slice(self, BufSlice_new_from_str(str));
}

ImmBuf *
ImmBuf_new_from_str(
    const char * str
) {
    return ImmBuf_new_from_slice(BufSlice_new_from_str(str));
}

ImmBuf *
ImmBuf_new_from_fmt(
    const char * fmt,
    ...
) {
    char fixed_buf[128];
    char * buf = fixed_buf;
    va_list args;
    ImmBuf * self;
    int len;

    va_start(args, fmt);

    len = vsnprintf(NULL, 0, fmt, args);

    if (len > sizeof(fixed_buf)) {
        buf = (char *)Mem_malloc(len + 1);
        if (buf == NULL) {
            va_end(args);
            return false;
        }
    }

    va_end(args);

    va_start(args, fmt);

    vsnprintf(buf, len + 1, fmt, args);

    va_end(args);

    self = ImmBuf_new_from_slice(BufSlice_new(buf, len));

    if (buf != fixed_buf) {
        Mem_free(buf);
    }

    return self;
}

ImmBuf *
ImmBuf_clone(
    ImmBuf * self
) {
    u8 * buf = NULL;
    usize len = 0;

    if (self->buf != NULL) {
        buf = (u8 *)Mem_malloc(self->len);
        if (buf == NULL) {
            goto Exit;
        }

        memcpy(buf, self->buf, self->len);
        len = self->len;
    }

    ImmBuf * copy = (ImmBuf *)
        Mem_malloc(sizeof(ImmBuf));
    if (copy == NULL) {
        goto FreeBuf;
    }

    copy->buf = buf;
    copy->len = len;

    return copy;

FreeBuf:
    if (buf != NULL) {
        Mem_free(buf);
    }

Exit:
    return NULL;
}

ImmBuf *
ImmBuf_new_null_terminated(
    ImmBuf * self
) {
    usize len = self->len + 1;
    ImmBuf * copy = ImmBuf_new(len);
    if (copy == NULL) {
        goto Exit;
    }

    memcpy(copy->buf, self->buf, self->len);
    copy->buf[self->len] = '\0';

    return copy;

Exit:
    return NULL;
}

ImmBuf *
ImmBuf_new_escaped(
    ImmBuf * self
) {
    BufSlice slice = ImmBuf_as_slice(self);
    usize len = BufSlice_escaped_len(slice);

    ImmBuf * str = ImmBuf_new(len);
    if (str == NULL) {
        goto Exit;
    }

    BufSlice_copy_escaped(slice, str->buf);

    return str;

Exit:
    return NULL;
}

/**
 * @brief Concatenate two buffers into a single buffer.
 */
ImmBuf *
ImmBuf_join(
    ImmBuf * obj_a,
    ImmBuf * obj_b
) {
    usize len = obj_a->len + obj_b->len;
    ImmBuf * buf = ImmBuf_new(len);
    if (buf == NULL) {
        return NULL;
    }

    memcpy(buf->buf, obj_a->buf, obj_a->len);
    memcpy(buf->buf + obj_a->len, obj_b->buf, obj_b->len);

    return buf;
}

bool
ImmBuf_compare(
    ImmBuf * self,
    ImmBuf * other
) {
    if (self->len != other->len ||
        memcmp(self->buf, other->buf, self->len) != 0) {

        return false;
    }

    return true;
}

bool
ImmBuf_compare_with_str(
    ImmBuf * self,
    const char * str
) {
    const u8 * str_buf = (const u8 *)str;
    usize str_len = strlen(str);
    if (self->len == str_len &&
        memcmp(self->buf, str_buf, str_len) == 0) {

        return true;
    }

    return false;
}

void
ImmBuf_to_uppercase(
    ImmBuf * self
) {
    for (usize i = 0; i < self->len; i++) {
        u8 byte = self->buf[i];
        if (byte >= 'a' &&
            byte <= 'z') {

            self->buf[i] = byte - 'a' + 'A';
        }
    }
}

BufSlice
ImmBuf_as_slice(
    const ImmBuf * self
) {
    return BufSlice_new(self->buf, self->len);
}

const u8 *
ImmBuf_data(
    ImmBuf * self
) {
    return self->buf;
}

u8 *
ImmBuf_data_mut(
    ImmBuf * self
) {
    return self->buf;
}

usize
ImmBuf_length(
    ImmBuf * self
) {
    return self->len;
}

void
ImmBuf_print(
    ImmBuf * self
) {
    if (self->buf == NULL ||
        self->len == 0) {
        return;
    }

    write(STDOUT_FILENO, self->buf, self->len);
}

void
ImmBuf_deinit(
    ImmBuf * self
) {
    if (self->buf != NULL) {
        Mem_free(self->buf);
    }
}

void
ImmBuf_free(
    ImmBuf * self
) {
    ImmBuf_deinit(self);
    Mem_free(self);
}