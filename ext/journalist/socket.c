#include "socket.h"

static pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;

FILE *journalist_file;
static char journalist_file_path_fmt[] = "/tmp/journalist-%d";

yajl_gen gen;

void
rb_journalist_socket_init() {

  gen = yajl_gen_alloc(NULL);
  yajl_gen_config(gen, yajl_gen_beautify, 0);

  pid_t pid = getpid();

  char path[1024];
  snprintf(
    path,
    1024,
    journalist_file_path_fmt,
    pid
  );

  journalist_file = fopen(path, "w");
}

void
rb_journalist_socket_send(int count, const char *event, ...)
{
  pthread_mutex_lock(&sock_mutex);

  yajl_gen_map_open(gen);

  yajl_gen_string(gen, (const unsigned char *)"event", 5);
  yajl_gen_string(gen, (const unsigned char *)event, strlen(event));

  if(count > 0) {
    va_list arguments;
    va_start(arguments, event);

    char         buf[32];
    const char  *key;
    int          type;
    size_t       length;

    char *string;
    int  integer;
    uint64_t uint64;

    int i;

    for(i = 0; i < count; i++) {
      key  = va_arg(arguments, char *);
      type = va_arg(arguments, int);

      yajl_gen_string(gen, (const unsigned char *)key, strlen(key));

      switch(type) {
        case 's':
          string = va_arg(arguments, char *);
          length = strlen(string);

          yajl_gen_string(gen, (const unsigned char *)string, length);
          break;

        case 'i':
          integer = va_arg(arguments, int);
          sprintf(buf, "%d", integer);
          length = strlen(buf);

          yajl_gen_number(gen, buf, length);
          break;

        case 'b':
          yajl_gen_bool(gen, va_arg(arguments, int));
          break;

        case 't':
          uint64 = va_arg(arguments, uint64_t);
          sprintf(buf, "%" PRIu64, uint64);
          length = strlen(buf);

          yajl_gen_number(gen, buf, length);
          break;
      }
    }
  }

  yajl_gen_map_close(gen);

  const unsigned char *buffer;
  size_t length;

  yajl_gen_get_buf(gen, &buffer, &length);

  fprintf(journalist_file, "%s\n", buffer);
  fflush(journalist_file);

  yajl_gen_clear(gen);
  yajl_gen_reset(gen, "");
  pthread_mutex_unlock(&sock_mutex);
}
