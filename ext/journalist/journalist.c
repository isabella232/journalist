#include "journalist.h"

VALUE rb_mJournalist;

static void
rb_journalist_init() {
  rb_journalist_socket_init();

  rb_journalist_gc_init();
  rb_journalist_calls_init();
  rb_journalist_allocations_init();
}

static VALUE
journalist_start(VALUE klass) {
  // rb_journalist_gc_start();
  // rb_journalist_calls_start();
  rb_journalist_allocations_start();

  return Qtrue;
}

static VALUE
journalist_stop(VALUE klass) {
  // rb_journalist_gc_stop();
  // rb_journalist_calls_stop();
  rb_journalist_allocations_stop();

  return Qtrue;
}

static VALUE
journalist_dump(VALUE klass) {
  rb_journalist_allocations_dump();

  return Qtrue;
}

void
Init_journalist(void)
{
  rb_mJournalist = rb_define_module("Journalist");

  rb_define_singleton_method(rb_mJournalist, "start", journalist_start, 0);
  rb_define_singleton_method(rb_mJournalist, "stop",  journalist_stop,  0);
  rb_define_singleton_method(rb_mJournalist, "dump",  journalist_dump,  0);

  rb_journalist_init();
}

uint64_t
timeofday_usec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec*1e6 +
         (uint64_t)tv.tv_usec;
}
