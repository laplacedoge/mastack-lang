#include "blob.h"

typedef struct _BlobObj {
    MutBuf buf;
} BlobObj;

void
BlobObj_init(
    BlobObj * self
) {
    MutBuf_init(&self->buf);
}

void
BlobObj_deinit(
    BlobObj * self
) {
    MutBuf_deinit(&self->buf);
}
