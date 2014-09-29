require "mkmf"

pkg_config('yajl')

$CFLAGS << " -Wall -Werror -Wno-unused-parameter "

create_makefile("journalist/journalist")
