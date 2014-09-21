#ifndef SOCKET_H
#define SOCKET_H 1

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "journalist.h"

void rb_journalist_socket_init();
void rb_journalist_socket_send(char *message);

#endif /* SOCKET_H */
