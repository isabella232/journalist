#ifndef GARBAGE_COLLECTION_H
#define GARBAGE_COLLECTION_H 1

#include "journalist.h"

void rb_journalist_gc_init();
void rb_journalist_gc_start();
void rb_journalist_gc_stop();

#endif /* GARBAGE_COLLECTION_H */
