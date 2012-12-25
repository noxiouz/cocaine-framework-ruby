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
VALUE dealer_new(VALUE cls, VALUE path);
//static VALUE dealer_send(VALUE self, VALUE message_hash);

template<typename T> 
static void dispose(void *ptr);

template<typename T>
T* get_ctype_pointer(VALUE from);

/****************************************************************************
 *  helpers
 */


template<typename T>
static void
dispose(void *ptr) {
    if (ptr) {
        T* _p = reinterpret_cast<T*>(ptr);
        delete _p;
    }
}

template<typename T>
T* 
get_ctype_pointer(VALUE from){
    /* 
     *  ADD throw() if out == NULL
     * */
    T* out;
    Data_Get_Struct(from, T, out);
    return out;
}


inline bool
Qbool_to_bool(VALUE condition){
    if (TYPE(condition) == T_TRUE){
        return true;
    } else if (TYPE(condition) == T_FALSE){
        return false;
    } else {
        rb_raise(rb_eArgError, "a boolean is required");
    }
}

/******************************************************************************/

VALUE 
dealer_new(VALUE cls, VALUE path){
    dealer_t* m_dealer = NULL; 
    try {
            m_dealer = new dealer_t(STR2CSTR(path));
    } catch(const internal_error& e) {
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    }
    VALUE t_data = Data_Wrap_Struct(cls, 0, dispose<dealer_t>, m_dealer);
    return t_data;
}

static VALUE
dealer_send(int argc, VALUE *argv, VALUE self){
    VALUE rb_message;
    VALUE rb_service;
    VALUE rb_handle;
    VALUE hash;

    rb_scan_args(argc, argv, "31", &rb_service, &rb_handle, &rb_message, &hash);

    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    message_policy_t policy = m_dealer->policy_for_service(STR2CSTR(rb_service));
        
    if (hash != Qnil){
        if (TYPE(hash) != T_HASH){
            rb_raise(rb_eArgError, "a hash is required");
        }
        VALUE hash_item;
        hash_item = rb_hash_aref(hash, rb_str_new2("deadline"));
        policy.deadline = NUM2DBL(hash_item);
        hash_item = rb_hash_aref(hash, rb_str_new2("timeout"));
        policy.timeout = NUM2DBL(hash_item);
        hash_item = rb_hash_aref(hash, rb_str_new2("urgent"));
        policy.urgent = Qbool_to_bool(hash_item);
        hash_item = rb_hash_aref(hash, rb_str_new2("persistent"));
        policy.persistent = Qbool_to_bool(hash_item);
    }
    response_holder_t* resp = NULL; 

    std::cout<<policy.as_string()<<std::endl;

    try {
        int size = 0;
        resp = new response_holder_t(m_dealer->send_message(
                                                            STR2CSTR(rb_message),
                                                            size,
                                                            message_path_t(STR2CSTR(rb_service), STR2CSTR(rb_handle)),
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
dealer_get_stored_messages_count(VALUE self, VALUE service_alias){
/*
 * TODO:  Add try .. catch
 */
    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    size_t s = m_dealer->stored_messages_count(STR2CSTR(service_alias));
    return INT2FIX(s);
}

static VALUE
dealer_get_stored_messages(VALUE self, VALUE service_alias){
/*
 * TODO:  Add try .. catch
 */
    std::vector<message_t> msgs;
    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    m_dealer->get_stored_messages(STR2CSTR(service_alias), msgs);
    VALUE messages_list = rb_ary_new();
    if (false == msgs.empty()){
           std::vector<message_t>::iterator it;
           for (it=msgs.begin(); it!=msgs.end(); ++it){
                VALUE t_data = Data_Wrap_Struct(cMessage, 0, dispose<message_holder>, new message_holder(*it));
                rb_ary_push(messages_list, t_data);
           }
           return messages_list;
    }
    return messages_list;
}

static VALUE
dealer_remove_stored_message(VALUE self, VALUE message){
/*
 * TODO:  Add try .. catch
 */
    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    message_holder* msg = get_ctype_pointer<message_holder>(message);
    m_dealer->remove_stored_message(msg->get());
    return Qtrue; 
}

/******************************************************************************/

extern "C"
void Init_Client(){
    /* Dealer dealer class */
    cClient = rb_define_class("Client", rb_cObject);
    rb_define_singleton_method(cClient, "new", (VALUE(*)(...))&dealer_new, 1);
    rb_define_method(cClient, "send", (VALUE(*)(...))&dealer_send, -1);
    rb_define_method(cClient, "get_msg_count", (VALUE(*)(...))&dealer_get_stored_messages_count, 1);
    rb_define_method(cClient, "get_messages", (VALUE(*)(...))&dealer_get_stored_messages, 1);
    rb_define_method(cClient, "remove_stored_message", (VALUE(*)(...))&dealer_remove_stored_message, 1);
    /* Response class */
    cResponse = rb_define_class("Response", rb_cObject);
    rb_define_method(cResponse, "get", (VALUE(*)(...))&response_get, 1);
    /* Message_t class*/
    cMessage = rb_define_class("Message", rb_cObject);
}
