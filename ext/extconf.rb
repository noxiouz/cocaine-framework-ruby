require 'mkmf'
have_library("stdc++")
have_library("cocaine-dealer")
find_header("c_dealer.hpp","./include/")
find_header("c_message.hpp")
find_header("c_response.hpp")

create_makefile("Client")
