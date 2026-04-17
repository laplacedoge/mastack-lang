#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "mut_buf.h"
#include "common/common.h"
#include "memory/memory.h"

static const usize INIT_CAP = 8;

void
MutBuf_init(
    MutBuf * self
) {
    self->buf = NULL;
    self->cap = 0;
    self->len = 0;
}

MutBuf *
MutBuf_new(void) {
    MutBuf * self = (MutBuf *)Mem_malloc(sizeof(MutBuf));
    if (self == NULL) {
        return NULL;
    }

    MutBuf_init(self);

    return self;
}

MutBuf *
MutBuf_new_from_buf(
    const void * buf,
    usize len
) {
    MutBuf * self = MutBuf_new();
    if (self == NULL) {
        goto Exit;
    }

    if (!MutBuf_push_buf(self, buf, len)) {
        goto FreeSelf;
    }

    return self;

FreeSelf:
    MutBuf_free(self);

Exit:
    return NULL;
}

MutBuf *
MutBuf_clone(
    MutBuf * self
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

    MutBuf * copy = (MutBuf *)Mem_malloc(sizeof(MutBuf));
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

bool
MutBuf_compare(
    MutBuf * self,
    MutBuf * other
) {
    if (self == other) {
        return true;
    }

    return self->len == other->len &&
        memcmp(self->buf, other->buf, self->len) == 0;
}

bool
MutBuf_resize(
    MutBuf * self,
    usize len
) {
    if (len <= self->cap) {
        self->len = len;
        return true;
    }

    u8 * buf;

    if (self->buf == NULL) {
        buf = Mem_malloc(len);
    } else {
        buf = Mem_realloc(self->buf, len);
    }

    if (buf == NULL) {
        return false;
    }

    self->buf = buf;
    self->cap = len;
    self->len = len;

    return true;
}

void
MutBuf_clear(
    MutBuf * self
) {
    self->len = 0;
}

bool
MutBuf_push(
    MutBuf * self,
    u8 byte
) {
    usize new_len = self->len + 1;

    if (new_len > self->cap) {
        usize new_cap = new_len;
        if (new_cap < INIT_CAP) {
            new_cap = INIT_CAP;
        } else {
            new_cap *= 2;
        }

        u8 * new_buf;
        if (self->buf == NULL) {
            new_buf = Mem_malloc(new_cap);
        } else {
            new_buf = Mem_realloc(self->buf, new_cap);
        }

        if (new_buf == NULL) {
            return false;
        }

        self->buf = new_buf;
        self->cap = new_cap;
    }

    self->buf[self->len] = byte;
    self->len += 1;

    return true;
}

bool
MutBuf_push_dup_byte(
    MutBuf * self,
    u8 byte,
    usize cnt
) {
    if (cnt == 0) {
        return true;
    }

    usize new_len = self->len + cnt;

    if (new_len > self->cap) {
        usize new_cap = new_len;
        if (new_cap < INIT_CAP) {
            new_cap = INIT_CAP;
        } else {
            new_cap *= 2;
        }

        u8 * new_buf;
        if (self->buf == NULL) {
            new_buf = Mem_malloc(new_cap);
        } else {
            new_buf = Mem_realloc(self->buf, new_cap);
        }

        if (new_buf == NULL) {
            return false;
        }

        self->buf = new_buf;
        self->cap = new_cap;
    }

    memset(self->buf + self->len, byte, cnt);
    self->len += cnt;

    return true;
}

bool
MutBuf_extend(
    MutBuf * self,
    BufSlice slice
) {
    return MutBuf_push_buf(self, slice.buf, slice.len);
}

bool
MutBuf_push_buf(
    MutBuf * self,
    const void * buf,
    usize len
) {
    if (len == 0) {
        return true;
    }

    usize required_len = self->len + len;
    if (required_len > self->cap) {
        usize required_cap = required_len;
        if (required_cap <= INIT_CAP) {
            required_cap = INIT_CAP;
        } else {
            required_cap *= 2;
        }

        u8 * required_buf;
        if (self->buf != NULL) {
            required_buf = (u8 *)Mem_realloc(
                self->buf, required_cap);
        } else {
            required_buf = Mem_malloc(required_cap);
        }
        if (required_buf == NULL) {
            return false;
        }

        self->buf = required_buf;
        self->cap = required_cap;
    }

    memcpy(self->buf + self->len, buf, len);

    self->len += len;

    return true;
}

bool
MutBuf_extend_with_str(
    MutBuf * self,
    const char * str
) {
    return MutBuf_push_buf(self, str, strlen(str));
}

bool
MutBuf_push_fmt(
    MutBuf * self,
    const char * fmt,
    ...
) {
    char fixed_buf[128];
    char * buf = fixed_buf;
    va_list args;
    bool result;
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

    result = MutBuf_push_buf(self, buf, len);

    if (buf != fixed_buf) {
        Mem_free(buf);
    }

    return result;
}

bool
MutBuf_extend_with_fmt_va(
    MutBuf * self,
    const char * fmt,
    va_list args
) {
    va_list args_measure;
    va_copy(args_measure, args);

    int len = vsnprintf(NULL, 0, fmt, args_measure);
    va_end(args_measure);
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

        vsnprintf(printed, allocated_len, fmt, args);

        BufSlice buf = BufSlice_new(printed, len);
        if (!MutBuf_extend(self, buf)) {
            goto FreeBuf;
        }

        res = true;

    FreeBuf:
        Mem_free(printed);

        return res;
    } else if (len > 0) {
        char printed[256];

        vsnprintf(printed, sizeof(printed), fmt, args);

        BufSlice buf = BufSlice_new(printed, len);
        if (!MutBuf_extend(self, buf)) {
            return false;
        }

        return true;
    } else {
        return true;
    }
}

void
MutBuf_left_shift(
    MutBuf * self,
    usize len
) {
    if (len >= self->len) {
        self->len = 0;
        return;
    }

    usize new_len = self->len - len;

    memmove(self->buf, self->buf + len, new_len);
    self->len = new_len;
}

MutBuf *
MutBuf_new_escaped(
    MutBuf * self
) {
    bool res;

    MutBuf * dst = MutBuf_new();
    if (dst == NULL) {
        goto Exit;
    }

    for (usize i = 0; i < self->len; i++) {
        u8 byte = self->buf[i];

        if (byte >= 0x20 &&
            byte <= 0x7E) {

            if (byte == '"') {
                res = MutBuf_push_buf(dst, "\\\"", 2);
            } else {
                res = MutBuf_push_buf(dst, &byte, 1);
            }
        } else {
            if (byte == '\t') {
                res = MutBuf_push_buf(dst, "\\t", 2);
            } else if (byte == '\n') {
                res = MutBuf_push_buf(dst, "\\n", 2);
            } else if (byte == '\r') {
                res = MutBuf_push_buf(dst, "\\r", 2);
            } else {
                const char * CHAR_MAP = "0123456789ABCDEF";
                u8 buf[4] = { '\\', 'x', '0', '0' };

                buf[2] = CHAR_MAP[byte >> 4];
                buf[3] = CHAR_MAP[byte & 0x0F];

                res = MutBuf_push_buf(dst, buf, sizeof(buf));
            }
        }

        if (!res) {
            goto FreeDst;
        }
    }

    return dst;

FreeDst:
    MutBuf_free(dst);

Exit:
    return NULL;
}

void
MutBuf_strip(
    MutBuf * self
) {
    for (ssize i = self->len - 1; i >= 0; i--) {
        u8 byte = self->buf[i];

        if (byte == ' ' ||
            byte == '\r' ||
            byte == '\n') {

            self->len -= 1;
        } else {
            break;
        }
    }

    usize anc = 0;

    for (usize i = 0; i < self->len; i++) {
        u8 byte = self->buf[i];

        if (byte == ' ' ||
            byte == '\r' ||
            byte == '\n') {

            anc += 1;
        } else {
            break;
        }
    }

    if (anc != 0) {
        memmove(self->buf, self->buf + anc, self->len - anc);
        self->len -= anc;
    }
}

bool
MutBuf_compact(
    MutBuf * self
) {
    if (self->buf == NULL) {
        return true;
    }

    if (self->len == self->cap) {
        return true;
    }

    if (self->len == 0) {
        Mem_free(self->buf);

        self->buf = NULL;
        self->cap = 0;
    } else {
        u8 * buf = (u8 *)Mem_realloc(self->buf, self->len);
        if (buf == NULL) {
            return false;
        }

        self->buf = buf;
        self->cap = self->len;
    }

    return true;
}

bool
MutBuf_merge(
    MutBuf * self,
    MutBuf * other
) {
    return MutBuf_push_buf(self, other->buf, other->len);
}

MutBuf *
MutBuf_join(
    MutBuf * self,
    MutBuf * other
) {
    MutBuf * dst = MutBuf_new();
    if (dst == NULL) {
        goto Exit;
    }

    if (!MutBuf_push_buf(dst, self->buf, self->len) ||
        !MutBuf_push_buf(dst, other->buf, other->len)) {

        goto FreeDst;
    }

    return dst;

FreeDst:
    MutBuf_free(dst);

Exit:
    return NULL;
}

void
MutBuf_print(
    MutBuf * self
) {
    if (self->buf == NULL ||
        self->len == 0) {
        return;
    }

    write(STDOUT_FILENO, self->buf, self->len);
}

void
MutBuf_deinit(
    MutBuf * self
) {
    if (self->buf != NULL) {
        Mem_free(self->buf);
    }
}

void
MutBuf_free(
    MutBuf * self
) {
    MutBuf_deinit(self);
    Mem_free(self);
}