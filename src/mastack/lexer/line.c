
#include "line.h"



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