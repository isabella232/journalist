#include "allocations.h"

#include "calls.h"

static struct {
  VALUE newobj;
  VALUE freeobj;
} allocations_tp_hook;

static struct {
  int       total;
  st_table *allocations;
  uint64_t  last_sent_at;
} aggregation;

typedef struct aggregation_entry_s {
  int        total;
  st_table  *locations;
} aggregation_entry_t;

static pthread_mutex_t alloc_mutex;
static pthread_t       alloc_observer;

static void
journalist_on_newobj(VALUE tpval, void *data) {
  rb_trace_arg_t *tparg = rb_tracearg_from_tracepoint(tpval);

  VALUE path   = rb_tracearg_path(tparg);
  VALUE lineno = rb_tracearg_lineno(tparg);
  VALUE obj    = rb_tracearg_object(tparg);

  // Initialize key strings for object type and file/line pair
  char *type_key = strdup(rb_obj_classname(obj));
  char *loc_key  = malloc(4096 * sizeof(char));
  sprintf(loc_key, "%s:%d", RSTRING_PTR(path), NUM2INT(lineno));

  // Start by looking up an entry.
  int lookup;
  aggregation_entry_t *entry = NULL;
  st_data_t entry_lookup;

  pthread_mutex_lock(&alloc_mutex);

  lookup = st_lookup(aggregation.allocations, (st_data_t)type_key, &entry_lookup);

  if(!lookup) {
    // If it doesn't exist, allocate memory for one and initialize it.
    entry = malloc(sizeof(aggregation_entry_t));
    entry->locations = st_init_strtable_with_size(64);
    entry->total = 0;

    st_insert(aggregation.allocations, (st_data_t)strdup(type_key), (st_data_t)entry);
  } else {
    // If it does exist, get the address.
    entry = (aggregation_entry_t *)entry_lookup;
  }

  entry->total++;

  // Get or initialize the allocations for
  int file_allocations = 0;
  lookup = st_lookup(entry->locations, (st_data_t)loc_key, (st_data_t *)&file_allocations);

  if(!lookup) file_allocations = 0;
  file_allocations++;

  st_insert(entry->locations, (st_data_t)strdup(loc_key), (st_data_t)file_allocations);

  aggregation.total++;

  pthread_mutex_unlock(&alloc_mutex);

  free(type_key);
  free(loc_key);
}

static void
journalist_on_freeobj(VALUE tpval, void *data) {
  // rb_journalist_socket_send("freeobj\n");
}

static int
journalist_allocations_send_locations(st_data_t key, st_data_t value, st_data_t data) {
  char *type     = (char *)data;
  char *location = (char *)key;
  int total      = (int)value;

  char message[4096];
  sprintf(message, "newobj_loc: type %s loc %s count %d\n",
    type, location, total);

  rb_journalist_socket_send(message);

  return ST_CONTINUE;
}

static int
journalist_allocations_send_types(st_data_t key, st_data_t value, st_data_t _) {
  char *type = (char *)key;
  aggregation_entry_t *entry = (aggregation_entry_t *)value;

  char message[4096];
  sprintf(message, "newobj: type %s count %d\n",
    type, entry->total);

  rb_journalist_socket_send(message);

  st_foreach(entry->locations, journalist_allocations_send_locations, key);

  return ST_CONTINUE;
}

void *
journalist_allocations_observer(void *threadid) {
  uint64_t current_time, last_sent_ago;
  do {
    current_time = timeofday_usec();
    last_sent_ago = current_time - aggregation.last_sent_at;

    pthread_mutex_lock(&alloc_mutex);
    if((last_sent_ago > 500000 && aggregation.total > 0) || aggregation.total > 100000) {
      st_foreach(aggregation.allocations, journalist_allocations_send_types, 0);
      aggregation.total = 0;
    }

    pthread_mutex_unlock(&alloc_mutex);
    usleep(500000);
  } while(1);

  return 0;
}

void
rb_journalist_allocations_init() {
  allocations_tp_hook.newobj  = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_NEWOBJ,  journalist_on_newobj,  0);
  allocations_tp_hook.freeobj = rb_tracepoint_new(0, RUBY_INTERNAL_EVENT_FREEOBJ, journalist_on_freeobj, 0);

  rb_gc_register_address(&allocations_tp_hook.newobj);
  rb_gc_register_address(&allocations_tp_hook.freeobj);

  aggregation.total = 0;
  aggregation.allocations = st_init_strtable_with_size(1024);
  aggregation.last_sent_at  = timeofday_usec();

  pthread_mutex_init(&alloc_mutex, 0);
}

void
rb_journalist_allocations_start() {
  rb_tracepoint_enable(allocations_tp_hook.newobj);
  rb_tracepoint_enable(allocations_tp_hook.freeobj);

  pthread_create(&alloc_observer, 0, journalist_allocations_observer, 0);
}

void
rb_journalist_allocations_stop() {
  rb_tracepoint_disable(allocations_tp_hook.newobj);
  rb_tracepoint_disable(allocations_tp_hook.freeobj);
}

void
rb_journalist_allocations_dump() {
  printf("DUMPING!\n");
}
