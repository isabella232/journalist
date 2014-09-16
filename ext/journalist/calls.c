#include "calls.h"

static struct {
  VALUE c_call;
  VALUE c_return;
} calls_tp_hook;

static void
journalist_on_call_c_call(VALUE tpval, void *data) {

  rb_journalist_socket_send("c_call\n");
}

static void
journalist_on_call_c_return(VALUE tpval, void *data) {
  rb_journalist_socket_send("c_return\n");
}

void
rb_journalist_calls_init() {
  calls_tp_hook.c_call   = rb_tracepoint_new(0, RUBY_EVENT_C_CALL,   journalist_on_call_c_call,   0);
  calls_tp_hook.c_return = rb_tracepoint_new(0, RUBY_EVENT_C_RETURN, journalist_on_call_c_return, 0);

  rb_gc_register_address(&calls_tp_hook.c_call);
  rb_gc_register_address(&calls_tp_hook.c_return);
}

void
rb_journalist_calls_start() {
  rb_tracepoint_enable(calls_tp_hook.c_call);
  rb_tracepoint_enable(calls_tp_hook.c_return);
}

void
rb_journalist_calls_stop() {
  rb_tracepoint_disable(calls_tp_hook.c_call);
  rb_tracepoint_disable(calls_tp_hook.c_return);
}
