#include "c_response.hpp"

using namespace cocaine::dealer;

VALUE
response_new(VALUE cls){
    rb_raise(rb_eNotImpError, "Response objects cannot be instantiated directly");
    return Qnil;
}

VALUE
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
