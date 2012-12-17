#include "ruby.h"
#include <cocaine/dealer/dealer.hpp>
#include <cocaine/dealer/utils/error.hpp>
#include "response_holder.hpp"

using namespace cocaine::dealer;

extern "C" void Init_Client();

/* Response class */
VALUE cResponse;
static VALUE response_get(VALUE self, VALUE _timeout);

/* Client class  */
VALUE cClient;
VALUE client_new(VALUE cls, VALUE path);
static VALUE client_send(VALUE self, VALUE service, VALUE handle, VALUE message);

template<typename T> 
static void dispose(void *ptr);

/****************************************************************************/

template<typename T>
static void
dispose(void *ptr) {
    if (ptr) {
        T* _p = reinterpret_cast<T*>(ptr);
        delete _p;
    }
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

/******************************************************************************/

VALUE 
client_new(VALUE cls, VALUE path){
    dealer_t* m_client = NULL; 
    try {
            m_client = new dealer_t(STR2CSTR(path));
    } catch(const internal_error& e) {
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    }
    VALUE t_data = Data_Wrap_Struct(cls, 0, dispose<dealer_t>, m_client);
    return t_data;
}

static VALUE
client_send(VALUE self, VALUE service, VALUE handle, VALUE message){
    int size = 0;
    dealer_t* m_client;
    Data_Get_Struct(self, dealer_t, m_client);
    message_policy_t policy = m_client->policy_for_service(STR2CSTR(service));
    response_holder_t* resp = NULL; 
    try {
        resp = new response_holder_t(m_client->send_message(
                                                            &message,
                                                            size,
                                                            message_path_t(STR2CSTR(service),
                                                            STR2CSTR(handle)),
                                                            policy
                                                        ));
    } catch(const dealer_error& e) {
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    } catch(const internal_error& e) {
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    }
    VALUE t_data = Data_Wrap_Struct(cResponse, 0, dispose<response_holder_t>, resp);
    return t_data;
}
/******************************************************************************/

extern "C"
void Init_Client(){
    /* Dealer client class */
    cClient = rb_define_class("Client", rb_cObject);
    rb_define_singleton_method(cClient, "new", (VALUE(*)(...))&client_new, 1);
    rb_define_method(cClient, "send", (VALUE(*)(...))&client_send, 3);
    /* Response class */
    cResponse = rb_define_class("Response", rb_cObject);
    rb_define_method(cResponse, "get", (VALUE(*)(...))&response_get, 1);
}
