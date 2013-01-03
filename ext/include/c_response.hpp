#ifndef _C_RESPONSE_HPP
#define _C_RESPONSE_HPP

#include <ruby.h>
#include <cocaine/dealer/dealer.hpp>
#include <response_holder.hpp>
#include <iostream>
using namespace cocaine::dealer;

static VALUE
response_new(VALUE cls){
    rb_raise(rb_eNotImpError, "Response objects cannot be instantiated directly");
    return Qnil;
}

static VALUE
response_get(VALUE self, VALUE _timeout){
    double timeout = NUM2DBL(_timeout);
    bool success = false;
    data_container chunk;

    response_holder_t* resp;
    Data_Get_Struct(self, response_holder_t, resp);

    success = resp->get()->get(&chunk, timeout);

    if (success) {
        return rb_str_new(static_cast<char*>(chunk.data()), chunk.size());
    } else {
        return rb_str_new2("");
    }

    return Qnil;
}

#endif
