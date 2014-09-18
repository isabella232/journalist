require "mkmf"

$CFLAGS << " -Wall -Werror -Wno-unused-parameter "

create_makefile("journalist/journalist")
