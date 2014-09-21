#include "socket.h"

static pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;

FILE *journalist_file;
static char journalist_file_path_fmt[] = "/tmp/journalist-%d";

void
rb_journalist_socket_init() {
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
rb_journalist_socket_send(char *message) {
  pthread_mutex_lock(&sock_mutex);

  fwrite(message, sizeof(char), strlen(message), journalist_file);
  fflush(journalist_file);

  pthread_mutex_unlock(&sock_mutex);
}
