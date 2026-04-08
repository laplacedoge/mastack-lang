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

bool
LexErr_write(
    LexErr * self,
    BufWriter * wrt
) {
    if (!BufWriter_write_fmt(wrt, "<\"%s\":", LexErrKind_to_cstr(self->kind))) {
        return false;
    }

    switch (self->kind) {
    case LexErrKind_InvalidByte: {
        SHORTEN_NAME(self->v.inv_byte, inner);
        if (!BufWriter_write_fmt(wrt, "0x%02X@(%zu,%zu)>", inner->byte, inner->row, inner->col)) {
            return false;
        }

        break;
    }
    }

    return true;
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

bool
LexRpt_write(
    LexRpt * self,
    BufWriter * wrt
) {
    bool res = false;

    usize cnt = LexRpt_count(self);
    if (!BufWriter_write_fmt(wrt, "<LexRpt(%zu): [", cnt)) {
        goto Exit;
    }

    if (cnt == 1) {
        LexErr * err = LexRpt_at(self, 0);
        if (!LexErr_write(err, wrt)) {
            goto Exit;
        }
    } else if (cnt > 1) {
        LexErr * err = LexRpt_at(self, 0);
        if (!LexErr_write(err, wrt)) {
            goto Exit;
        }

        for (usize i = 1; i < cnt; i++) {
            LexErr * err = LexRpt_at(self, i);
            if (!BufWriter_write_str(wrt, ", ") ||
                !LexErr_write(err, wrt)) {

                goto Exit;
            }
        }
    }

    if (!BufWriter_write_str(wrt, "]>")) {
        goto Exit;
    }

    res = true;

Exit:
    return res;
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