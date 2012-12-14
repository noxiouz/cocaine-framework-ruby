#include "ruby.h"
#include "cocaine/dealer/dealer.hpp"
#include "boost/shared_ptr.hpp"

static VALUE
hello(VALUE self) {
    return Qnil;
}
/* Response class */
VALUE cResponse;

using namespace cocaine::dealer;

class response_holder_t{
    public:
        explicit response_holder_t(const boost::shared_ptr<response_t>& response_):
                        m_response(response_)
                {}

        response_t* operator -> (){
                return m_response.get();
        }
        response_t* get(){
            return m_response.get();
        }
    private:
        boost::shared_ptr<response_t> m_response;
};

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
        return rb_str_new2("EMPTY");
    }

    return Qnil;
}

/******************************************************************************/

/* Client class  */
VALUE cClient;

static void
client_free(void *ptr) {
    using namespace cocaine::dealer;
    if (ptr) {
        dealer_t* _p = reinterpret_cast<dealer_t*>(ptr);
        delete _p;
    }
}

VALUE 
client_new(VALUE cls, VALUE path){
    using namespace cocaine::dealer;
    dealer_t* m_client = new dealer_t(STR2CSTR(path));
    VALUE t_data = Data_Wrap_Struct(cls, 0, client_free, m_client);
    return t_data;
}

static VALUE
client_send(VALUE self, VALUE service, VALUE handle, VALUE message){
    using namespace cocaine::dealer;
    int size = 0;
    dealer_t* m_client;
    Data_Get_Struct(self, dealer_t, m_client);
    message_policy_t policy = m_client->policy_for_service(STR2CSTR(service));
    response_holder_t* resp = new response_holder_t(m_client->send_message(
            &message,
            size,
            message_path_t(STR2CSTR(service), STR2CSTR(handle)),
            policy
        ));
    VALUE t_data = Data_Wrap_Struct(cResponse, 0, 0, resp);
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