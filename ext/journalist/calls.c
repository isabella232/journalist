#include "calls.h"

static struct {
  VALUE c_call;
  VALUE c_return;
} calls_tp_hook;

// TODO: For now, a very basic counter. What we really want to do is keep around
// the trace argument, and keep track of what's going on at all times (maybe
// even benchmark how long calls take and only output the info on return.)
static int stack_size = 0;

const char c_call_fmt[] = "c_call: "
  "class %s method: %s class_method %d stack_depth %d "
  "path %s line %d\n";

static void
journalist_on_call_c_call(VALUE tpval, void *data) {
  rb_trace_arg_t *tparg = rb_tracearg_from_tracepoint(tpval);

  VALUE self      = rb_tracearg_self(tparg);
  VALUE method_id = rb_tracearg_method_id(tparg);
  VALUE path      = rb_tracearg_path(tparg);
  VALUE lineno    = rb_tracearg_lineno(tparg);
  bool singleton  = TYPE(self) == T_CLASS || TYPE(self) == T_MODULE;

  char buffer[4096];
  sprintf(buffer,
    c_call_fmt,
    rb_obj_classname(self),
    rb_id2name(SYM2ID(method_id)),
    singleton,
    stack_size,
    RSTRING_PTR(path),
    NUM2INT(lineno)
  );

  rb_journalist_socket_send(buffer);
  stack_size++;
}

static void
journalist_on_call_c_return(VALUE tpval, void *data) {
  if(stack_size == 0) return;

  rb_journalist_socket_send("c_return\n");
  stack_size--;
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
