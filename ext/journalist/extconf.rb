require "mkmf"

yajl_inc, yajl_lib = pkg_config('yajl')
abort "----\nCannot find YAJL\n----" unless yajl_inc && yajl_lib

$CFLAGS << " -Wall -Werror -Wno-unused-parameter "

create_makefile("journalist/journalist")
