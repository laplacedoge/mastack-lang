#include "line.h"

bool
LineInfo_write(
    LineInfo * self,
    BufWriter * wrt
) {
    const char * eol;
    switch (self->eol) {
    case Eol_None: eol = "NONE"; break;
    case Eol_Cr: eol = "CR"; break;
    case Eol_Lf: eol = "LF"; break;
    case Eol_CrLf: eol = "CRLF"; break;
    }
    return BufWriter_write_fmt(wrt, "<@%zu+%zu|%s>", self->off, self->len, eol);
}

void
LineInfo_deinit(
    LineInfo * self
) {

}

void
LineCache_init(
    LineCache * self
) {
    MutBuf_init(&self->buf);
    self->cnt = 0;
}

bool
LineCache_push(
    LineCache * self,
    LineInfo * info
) {
    BufSlice buf = BufSlice_new_from_ptr(info);
    if (!MutBuf_extend(&self->buf, buf)) {
        return false;
    }

    self->cnt += 1;

    return true;
}

bool
LineCache_write(
    LineCache * self,
    BufWriter * wrt
) {
    bool res = false;

    usize cnt = LineCache_count(self);
    if (!BufWriter_write_fmt(wrt, "<LineInfo(%zu): [", cnt)) {
        goto Exit;
    }

    if (cnt == 1) {
        LineInfo * info = LineCache_at(self, 0);
        if (!LineInfo_write(info, wrt)) {
            goto Exit;
        }
    } else if (cnt > 1) {
        LineInfo * info = LineCache_at(self, 0);
        if (!LineInfo_write(info, wrt)) {
            goto Exit;
        }

        for (usize i = 1; i < cnt; i++) {
            LineInfo * info = LineCache_at(self, i);
            if (!BufWriter_write_str(wrt, ", ") ||
                !LineInfo_write(info, wrt)) {

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
LineCache_clear(
    LineCache * self
) {
    for (usize i = 0; i < self->cnt; i++) {
        LineInfo_deinit(LineCache_at(self, i));
    }
}

void
LineCache_deinit(
    LineCache * self
) {
    LineCache_clear(self);
    MutBuf_deinit(&self->buf);
}