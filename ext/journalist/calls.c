#include "calls.h"

static struct {
  VALUE c_call;
  VALUE c_return;
} calls_tp_hook;

static uint64_t call_times[MAX_CALLS] = {0};
static int call_count = 0;

const char slow_cpu_fmt[] = "slow_cpu: "
  "class %s method %s class_method %d stack_depth %d "
  "path %s line %d time %" PRIu64 "\n";

static void
journalist_on_call_c_call(VALUE tpval, void *data) {
  call_times[call_count] = ru_utime_usec();
  call_count++;
}

static void
journalist_on_call_c_return(VALUE tpval, void *data) {
  if(call_count == 0) return;
  call_count--;

  uint64_t diff = ru_utime_usec() - call_times[call_count];
  if(diff < 250000) return;

  rb_trace_arg_t *tparg = rb_tracearg_from_tracepoint(tpval);

  VALUE self      = rb_tracearg_self(tparg);
  VALUE method_id = rb_tracearg_method_id(tparg);
  VALUE path      = rb_tracearg_path(tparg);
  VALUE lineno    = rb_tracearg_lineno(tparg);
  bool singleton  = TYPE(self) == T_CLASS || TYPE(self) == T_MODULE;

  char buffer[4096];
  sprintf(buffer,
    slow_cpu_fmt,
    rb_obj_classname(self),
    rb_id2name(SYM2ID(method_id)),
    singleton,
    call_count,
    RSTRING_PTR(path),
    NUM2INT(lineno),
    diff
  );

  rb_journalist_socket_send(buffer);
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
