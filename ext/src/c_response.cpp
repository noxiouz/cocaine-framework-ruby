#include "c_response.hpp"
#include "cocaine/dealer/utils/error.hpp"

using namespace cocaine::dealer;

VALUE
response_new(VALUE cls){
    rb_raise(rb_eNotImpError, "Response objects cannot be instantiated directly");
    return Qnil;
}

VALUE
response_get(VALUE self, VALUE _timeout){
    RUBY_TRY
    double timeout = NUM2DBL(_timeout);
    bool success = false;
    data_container chunk;

    response_holder_t* resp;
    Data_Get_Struct(self, response_holder_t, resp);
    try {
        success = resp->get()->get(&chunk, timeout);
    } catch(const dealer_error& e){
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    } catch(const internal_error& e){
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    }

    if (success) {
        return rb_str_new(static_cast<char*>(chunk.data()), chunk.size());
    } else {
        return rb_str_new2("");
    }
    return Qnil;
    RUBY_CATCH
}
