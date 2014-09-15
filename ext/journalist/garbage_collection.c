#include "garbage_collection.h"


static struct {
  VALUE start;
  VALUE end;
} gc_tp_hook;

static struct {
  VALUE gc;
} cookie;

VALUE sym_count;
VALUE sym_heap_used;
VALUE sym_heap_length;

void
rb_journalist_gc_init() {
  // Initialize symbols
#define S(name) sym_##name = ID2SYM(rb_intern(#name));
  S(count);
  S(heap_used);
  S(heap_length);
#undef S

  // Garbage Collection
  cookie.gc = rb_hash_new();
  rb_gc_register_mark_object(cookie.gc);
  rb_gc_stat(cookie.gc);

  gc_tp_hook.start = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_GC_START,     journalist_on_gc_start, 0);
  gc_tp_hook.end   = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_GC_END_SWEEP, journalist_on_gc_end,   0);

  rb_gc_register_address(&gc_tp_hook.start);
  rb_gc_register_address(&gc_tp_hook.end);
}

void
rb_journalist_gc_start() {
  rb_tracepoint_enable(gc_tp_hook.start);
  rb_tracepoint_enable(gc_tp_hook.end);
}

void
rb_journalist_gc_stop() {
  rb_tracepoint_disable(gc_tp_hook.start);
  rb_tracepoint_disable(gc_tp_hook.end);
}

static void
journalist_on_gc_start(VALUE tpval, void *data) {
}

static void
journalist_on_gc_end(VALUE tpval, void *data) {
  rb_gc_stat(cookie.gc);

#define STAT(name) NUM2INT(rb_hash_aref(cookie.gc, sym_##name))
  char buffer[4096];
  sprintf(buffer, "gc_end: count %d heap_used %d heap_length %d\n",
    STAT(count),
    STAT(heap_used),
    STAT(heap_length)
  );
#undef STAT

  rb_journalist_socket_send(buffer);
}
