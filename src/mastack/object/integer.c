#include "memory/memory.h"
#include "integer.h"

typedef struct _IntObj {
    ssize val;
} IntObj;

void
IntObj_init(
    IntObj * self,
    ssize val
) {
    self->val = val;
}

IntObj *
IntObj_new(
    ssize val
) {
    IntObj * self = (IntObj *)Mem_malloc(sizeof(IntObj));
    if (self == NULL) {
        return NULL;
    }

    IntObj_init(self, val);

    return self;
}

void
IntObj_plus(
    IntObj * self,
    IntObj * other,
    IntObj * dst
) {
    dst->val = self->val + other->val;
}

void
IntObj_minus(
    IntObj * self,
    IntObj * other,
    IntObj * dst
) {
    dst->val = self->val - other->val;
}

void
IntObj_times(
    IntObj * self,
    IntObj * other,
    IntObj * dst
) {
    dst->val = self->val * other->val;
}

void
IntObj_divided_by(
    IntObj * self,
    IntObj * other,
    IntObj * dst
) {
    dst->val = self->val / other->val;
}

void
IntObj_deinit(
    IntObj * self
) {

}

void
IntObj_free(
    IntObj * self
) {
    IntObj_deinit(self);
    Mem_free(self);
}