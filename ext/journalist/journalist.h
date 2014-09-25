#ifndef JOURNALIST_H
#define JOURNALIST_H 1

#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "ruby.h"
#include "ruby/debug.h"

#include "allocations.h"
#include "calls.h"
#include "garbage_collection.h"
#include "socket.h"

uint64_t timeofday_usec();
uint64_t ru_utime_usec();
#endif /* JOURNALIST_H */
