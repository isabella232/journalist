#include "socket.h"

FILE *journalist_file;
static char journalist_file_path[] = "/tmp/journalist";

void
rb_journalist_socket_init() {
  journalist_file = fopen(journalist_file_path, "w");
}

void
rb_journalist_socket_send(char *message) {
  fwrite(message, sizeof(char), strlen(message), journalist_file);
  fflush(journalist_file);
}
