#ifndef __MASTACK_OBJECT_INTEGER_H__
#define __MASTACK_OBJECT_INTEGER_H__

#include "common/common.h"

typedef struct _IntObj IntObj;

void
IntObj_init(
    IntObj * self,
    ssize val
);

IntObj *
IntObj_new(
    ssize val
);

void
IntObj_plus(
    IntObj * self,
    IntObj * other,
    IntObj * dst
);

void
IntObj_minus(
    IntObj * self,
    IntObj * other,
    IntObj * dst
);

void
IntObj_times(
    IntObj * self,
    IntObj * other,
    IntObj * dst
);

void
IntObj_divided_by(
    IntObj * self,
    IntObj * other,
    IntObj * dst
);

void
IntObj_deinit(
    IntObj * self
);

void
IntObj_free(
    IntObj * self
);

#endif