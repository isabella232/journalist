#ifndef SOCKET_H
#define SOCKET_H 1

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "yajl_gen.h"

#include "journalist.h"

void rb_journalist_socket_init();
void rb_journalist_socket_send(int count, char *event, ...);

#endif /* SOCKET_H */
