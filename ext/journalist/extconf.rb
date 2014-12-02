require "mkmf"

ruby = RUBY_VERSION.split('.').map{ |s| s.to_i }
if ruby[0] < 2 || (ruby[0] == 2 && ruby[1] < 1)
  abort "----\nJournalist requires MRI >= 2.1\n----"
end

yajl_inc, yajl_lib = pkg_config('yajl')
abort "----\nCannot find YAJL\n----" unless yajl_inc && yajl_lib

$CFLAGS << " -Wall -Werror -Wno-unused-parameter "

create_makefile("journalist/journalist")
