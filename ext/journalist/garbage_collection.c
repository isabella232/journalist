#include "garbage_collection.h"

static struct {
  VALUE start;
  VALUE end_mark;
  VALUE end_sweep;
} gc_tp_hook;

static struct {
  VALUE gc;
} cookie;

static VALUE sym_count,
  sym_heap_used,
  sym_heap_length,
  sym_heap_increment,
  sym_heap_live_slot,
  sym_heap_free_slot,
  sym_heap_final_slot,
  sym_heap_swept_slot,
  sym_total_allocated_object,
  sym_total_freed_object,
  sym_malloc_increase,
  sym_malloc_limit,
  sym_minor_gc_count,
  sym_major_gc_count,
  sym_remembered_shady_object,
  sym_remembered_shady_object_limit,
  sym_old_object,
  sym_old_object_limit,
  sym_oldmalloc_increase,
  sym_oldmalloc_limit;

const char gc_end_sweep_fmt[] = "gc_end_sweep: "
  "count %d heap_used %d heap_length %d heap_live_slot %d heap_free_slot %d "
  "heap_final_slot %d heap_swept_slot %d total_allocated_object %d "
  "total_freed_object %d malloc_increase %d malloc_limit %d "
  "minor_gc_count %d major_gc_count %d remembered_shady_object %d "
  "remembered_shady_object_limit %d old_object %d old_object_limit %d "
  "oldmalloc_increase %d oldmalloc_limit %d\n";

static void
journalist_on_gc_start(VALUE tpval, void *data) {
  rb_journalist_socket_send("gc_start\n");
}

static void
journalist_on_gc_end_mark(VALUE tpval, void *data) {
  rb_journalist_socket_send("gc_mark\n");
}

static void
journalist_on_gc_end_sweep(VALUE tpval, void *data) {
  char buffer[4096];
  rb_gc_stat(cookie.gc);

#define STAT(name) NUM2INT(rb_hash_aref(cookie.gc, sym_##name))
  sprintf(buffer,
    gc_end_sweep_fmt,
    STAT(count),
    STAT(heap_used),
    STAT(heap_length),
    STAT(heap_live_slot),
    STAT(heap_free_slot),
    STAT(heap_final_slot),
    STAT(heap_swept_slot),
    STAT(total_allocated_object),
    STAT(total_freed_object),
    STAT(malloc_increase),
    STAT(malloc_limit),
    STAT(minor_gc_count),
    STAT(major_gc_count),
    STAT(remembered_shady_object),
    STAT(remembered_shady_object_limit),
    STAT(old_object),
    STAT(old_object_limit),
    STAT(oldmalloc_increase),
    STAT(oldmalloc_limit)
  );
#undef STAT

  rb_journalist_socket_send(buffer);
}

void
rb_journalist_gc_init() {
  // Initialize symbols
#define S(name) sym_##name = ID2SYM(rb_intern_const(#name));
  S(count);
  S(heap_used);
  S(heap_length);
  S(heap_increment);
  S(heap_live_slot);
  S(heap_free_slot);
  S(heap_final_slot);
  S(heap_swept_slot);
  S(total_allocated_object);
  S(total_freed_object);
  S(malloc_increase);
  S(malloc_limit);
  S(minor_gc_count);
  S(major_gc_count);
  S(remembered_shady_object);
  S(remembered_shady_object_limit);
  S(old_object);
  S(old_object_limit);
  S(oldmalloc_increase);
  S(oldmalloc_limit);
#undef S

  // Grab the latest info on GC. We need to do this so that Ruby does all the
  // allocations it needs for this object outside of GC, so that we don't
  // blow up the interpreter. Also, mark it so that GC doesn't reap it on a run.
  cookie.gc = rb_hash_new();
  rb_gc_register_mark_object(cookie.gc);
  rb_gc_stat(cookie.gc);

  gc_tp_hook.start     = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_GC_START,     journalist_on_gc_start,     0);
  gc_tp_hook.end_mark  = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_GC_END_MARK,  journalist_on_gc_end_mark,  0);
  gc_tp_hook.end_sweep = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_GC_END_SWEEP, journalist_on_gc_end_sweep, 0);

  rb_gc_register_address(&gc_tp_hook.start);
  rb_gc_register_address(&gc_tp_hook.end_mark);
  rb_gc_register_address(&gc_tp_hook.end_sweep);
}

void
rb_journalist_gc_start() {
  rb_tracepoint_enable(gc_tp_hook.start);
  rb_tracepoint_enable(gc_tp_hook.end_mark);
  rb_tracepoint_enable(gc_tp_hook.end_sweep);
}

void
rb_journalist_gc_stop() {
  rb_tracepoint_disable(gc_tp_hook.start);
  rb_tracepoint_disable(gc_tp_hook.end_mark);
  rb_tracepoint_disable(gc_tp_hook.end_sweep);
}
