#ifndef JOURNALIST_H
#define JOURNALIST_H 1

#include "ruby.h"
#include "ruby/debug.h"

#include "garbage_collection.h"
#include "socket.h"

static void rb_journalist_init();

static VALUE journalist_start(VALUE klass);
static VALUE journalist_stop(VALUE klass);

#endif /* JOURNALIST_H */
