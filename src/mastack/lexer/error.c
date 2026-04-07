#include "error.h"

const char *
LexErrKind_to_cstr(
    LexErrKind kind
) {
    switch (kind) {
    case LexErrKind_InvalidByte: return "Invalid byte";
    }
}

void
LexErr_init_invalid_byte(
    LexErr * self,
    usize row,
    usize col,
    u8 byte
) {
    self->kind = LexErrKind_InvalidByte;

    SHORTEN_NAME(self->v.inv_byte, inner);
    inner->row = row;
    inner->col = col;
    inner->byte = byte;
}

void
LexErr_deinit(
    LexErr * self
) {
    switch (self->kind) {
    case LexErrKind_InvalidByte:
    default:
        break;
    }
}

void
LexRpt_init(
    LexRpt * self
) {
    MutBuf_init(&self->buf);
    self->cnt = 0;
}

bool
LexRpt_add(
    LexRpt * self,
    LexErr * err
) {
    BufSlice buf = BufSlice_new_from_ptr(err);
    if (!MutBuf_extend(&self->buf, buf)) {
        return false;
    }

    self->cnt += 1;

    return true;
}

bool
LexRpt_add_error_invalid_byte(
    LexRpt * self,
    usize row,
    usize col,
    u8 byte
) {
    LexErr err;
    LexErr_init_invalid_byte(&err, row, col, byte);
    return LexRpt_add(self, &err);
}

void
LexRpt_clear(
    LexRpt * self
) {
    LexRpt_for_each(self, err, {
        LexErr_deinit(err);
    });

    self->cnt = 0;
}

void
LexRpt_deinit(
    LexRpt * self
) {
    LexRpt_clear(self);
    MutBuf_deinit(&self->buf);
}