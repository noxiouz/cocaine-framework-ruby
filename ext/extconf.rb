require 'mkmf'

library_name = "Client"

have_library("stdc++")
have_library("cocaine-dealer")
find_header("c_dealer.hpp","./include/")

create_makefile(library_name, 'src')
