#ifndef GARBAGE_COLLECTION_H
#define GARBAGE_COLLECTION_H 1

#include "journalist.h"

void rb_journalist_gc_init();
void rb_journalist_gc_start();
void rb_journalist_gc_stop();

static void journalist_on_gc_start(VALUE tpval, void *data);
static void journalist_on_gc_end(VALUE tpval, void *data);

#endif /* GARBAGE_COLLECTION_H */
