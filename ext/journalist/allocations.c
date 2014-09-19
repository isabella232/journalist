#include "allocations.h"

#include "calls.h"

static struct {
  VALUE newobj;
  VALUE freeobj;
} allocations_tp_hook;

const char newobj_fmt[] = "newobj: "
  "type %s class %s method %s class_method %d path %s line %d\n";

static void
journalist_on_newobj(VALUE tpval, void *data) {
  rb_trace_arg_t *tparg = rb_tracearg_from_tracepoint(tpval);

  VALUE self      = rb_tracearg_self(tparg);
  VALUE method_id = rb_tracearg_method_id(tparg);
  VALUE path      = rb_tracearg_path(tparg);
  VALUE lineno    = rb_tracearg_lineno(tparg);
  VALUE obj       = rb_tracearg_object(tparg);
  bool singleton  = TYPE(self) == T_CLASS || TYPE(self) == T_MODULE;

  if (TYPE(self) == T_ICLASS)
    self = RBASIC_CLASS(self);

  char buffer[4096];
  sprintf(buffer,
    newobj_fmt,
    rb_obj_classname(obj),
    rb_obj_classname(self),
    rb_id2name(SYM2ID(method_id)),
    singleton,
    RSTRING_PTR(path),
    NUM2INT(lineno)
  );

  rb_journalist_socket_send(buffer);
}

static void
journalist_on_freeobj(VALUE tpval, void *data) {
  rb_journalist_socket_send("freeobj\n");
}

void
rb_journalist_allocations_init() {
  allocations_tp_hook.newobj  = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_NEWOBJ,  journalist_on_newobj,  0);
  allocations_tp_hook.freeobj = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_FREEOBJ, journalist_on_freeobj, 0);

  rb_gc_register_address(&allocations_tp_hook.newobj);
  rb_gc_register_address(&allocations_tp_hook.freeobj);
}

void
rb_journalist_allocations_start() {
  rb_tracepoint_enable(allocations_tp_hook.newobj);
  rb_tracepoint_enable(allocations_tp_hook.freeobj);
}

void
rb_journalist_allocations_stop() {
  rb_tracepoint_disable(allocations_tp_hook.newobj);
  rb_tracepoint_disable(allocations_tp_hook.freeobj);
}
