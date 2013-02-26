#ifndef _C_RESPONSE_HPP
#define _C_RESPONSE_HPP

#include <ruby.h>
#include <cocaine/dealer/dealer.hpp>
#include <response_holder.hpp>
#include "helpers.hpp"
#include <iostream>

VALUE
response_new(VALUE cls);

VALUE
response_get(VALUE self, VALUE _timeout);
#endif
