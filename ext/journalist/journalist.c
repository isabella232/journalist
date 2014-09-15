#include "journalist.h"

VALUE rb_mJournalist;

void
Init_journalist(void)
{
  rb_mJournalist = rb_define_module("Journalist");

  rb_define_singleton_method(rb_mJournalist, "start", journalist_start, 0);
  rb_define_singleton_method(rb_mJournalist, "stop",  journalist_stop,  0);

  rb_journalist_init();
}

static void
rb_journalist_init() {
  rb_journalist_socket_init();

  rb_journalist_gc_init();
}

static VALUE
journalist_start(VALUE klass) {
  rb_journalist_gc_start();

  return Qtrue;
}

static VALUE
journalist_stop(VALUE klass) {
  rb_journalist_gc_stop();

  return Qtrue;
}