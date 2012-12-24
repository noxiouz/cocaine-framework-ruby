#include "ruby.h"
#include <cocaine/dealer/dealer.hpp>
#include <cocaine/dealer/utils/error.hpp>

#include "c_response.hpp"
#include "c_message.hpp"

using namespace cocaine::dealer;

extern "C" void Init_Client();

/* Message_t class */

VALUE cMessage;

/* Response class */
VALUE cResponse;

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

template<typename T>
static T*
get_pointer(VALUE from){
    T* out;
    Data_Get_Struct(from, T, out);
    return out;
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

static VALUE
client_get_stored_messages_count(VALUE self, VALUE service_alias){
/*
 * TODO:  Add try .. catch
 */
    dealer_t* m_client;
    Data_Get_Struct(self, dealer_t, m_client);
    size_t s = m_client->stored_messages_count(STR2CSTR(service_alias));
    return INT2FIX(s);
}

static VALUE
client_get_stored_messages(VALUE self, VALUE service_alias){
    std::vector<message_t> msgs;
    dealer_t* m_client;
    Data_Get_Struct(self, dealer_t, m_client);
    m_client->get_stored_messages(STR2CSTR(service_alias), msgs);
    if (false == msgs.empty()){
           VALUE messages_list = rb_ary_new();
           std::vector<message_t>::iterator it;
           message_holder* _msg;
           for (it=msgs.begin(); it!=msgs.end(); ++it){
                VALUE t_data = Data_Wrap_Struct(cMessage, 0, dispose<message_holder>, _msg);
                rb_ary_push(messages_list, t_data);
                return messages_list;
           }
    }
    return Qnil;
}

static VALUE
client_remove_stored_message(VALUE self, VALUE message){
    dealer_t* m_client = get_pointer<dealer_t>(self);
    message_holder* msg = get_pointer<message_holder>(message);
    if (msg){
        std::cout<<"OKFFFFFFFFF";
    }
    //m_client->remove_stored_message(msg->get());
    return Qnil; 
}

/******************************************************************************/

extern "C"
void Init_Client(){
    /* Dealer client class */
    cClient = rb_define_class("Client", rb_cObject);
    rb_define_singleton_method(cClient, "new", (VALUE(*)(...))&client_new, 1);
    rb_define_method(cClient, "send", (VALUE(*)(...))&client_send, 3);
    rb_define_method(cClient, "get_msg_count", (VALUE(*)(...))&client_get_stored_messages_count, 1);
    rb_define_method(cClient, "get_messages", (VALUE(*)(...))&client_get_stored_messages, 1);
    rb_define_method(cClient, "remove_stored_message", (VALUE(*)(...))&client_remove_stored_message, 1);
    /* Response class */
    cResponse = rb_define_class("Response", rb_cObject);
    rb_define_method(cResponse, "get", (VALUE(*)(...))&response_get, 1);
    /* Message_t class*/
    cMessage = rb_define_class("Message", rb_cObject);
}
