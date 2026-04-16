#include <string.h>

#include "common.h"

BufSlice
BufSlice_new_from_cstr(
    const char * str
) {
    return BufSlice_new(str, strlen(str));
}

usize
BufSlice_escaped_len(
    BufSlice self
) {
    usize escaped_len = 0;

    for (usize i = 0; i < self.len; i++) {
        u8 byte = self.buf[i];

        if (byte >= 0x20 &&
            byte <= 0x7E) {

            switch (byte) {
            case '"':
            case '\'':
            case '\\':
                escaped_len += 2;
                break;

            default:
                escaped_len += 1;
                break;
            }
        } else {
            switch (byte) {
            case '\a':
            case '\b':
            case '\t':
            case '\n':
            case '\v':
            case '\f':
            case '\r':
            case '\e':
                escaped_len += 2;
                break;

            default:
                escaped_len += 4;
                break;
            }
        }
    }

    return escaped_len;
}

void
BufSlice_copy_escaped(
    BufSlice self,
    u8 * buf
) {
    for (usize i = 0; i < self.len; i++) {
        u8 byte = self.buf[i];

        if (byte >= 0x20 &&
            byte <= 0x7E) {

            switch (byte) {
            case '"':   *buf++ = '\\'; *buf++ = '"'; break;
            case '\'':  *buf++ = '\\'; *buf++ = '\''; break;
            case '\\':  *buf++ = '\\'; *buf++ = '\\'; break;
            default:    *buf++ = byte; break;
            }
        } else {
            *buf++ = '\\';

            switch (byte) {
            case '\a':  *buf++ = 'a'; break;
            case '\b':  *buf++ = 'b'; break;
            case '\t':  *buf++ = 't'; break;
            case '\n':  *buf++ = 'n'; break;
            case '\v':  *buf++ = 'v'; break;
            case '\f':  *buf++ = 'f'; break;
            case '\r':  *buf++ = 'r'; break;
            case '\e':  *buf++ = 'e'; break;
            default: {
                const char * CHAR_MAP = "0123456789ABCDEF";

                *buf++ = 'x';
                *buf++ = CHAR_MAP[byte >> 4];
                *buf++ = CHAR_MAP[byte & 0x0F];

                break;
            }
            }
        }
    }
}