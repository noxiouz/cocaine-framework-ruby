#include "c_dealer.hpp"
#include "c_response.hpp"
#include "c_message.hpp"
#include "helpers.hpp"
#include "module.hpp"

#include <cocaine/dealer/dealer.hpp>
#include <cocaine/dealer/utils/error.hpp>


#define RUBY_TRY \
    extern VALUE ruby_errinfo; \
    ruby_errinfo = Qnil; \
    try {

#define RUBY_CATCH \
    } catch(const std::exception &e) { \
        std::ostringstream o; \
        o << "c++error: " << e.what(); \
        ruby_errinfo = rb_exc_new2( \
        rb_eRuntimeError, o.str().c_str()); \
    } catch(...) { \
        ruby_errinfo = rb_exc_new2( \
         rb_eRuntimeError, "c++error: Unknown error"); \
    } \
      if(!NIL_P(ruby_errinfo)) { \
      rb_exc_raise(ruby_errinfo); \
    }

using namespace cocaine::dealer;

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

VALUE
dealer_send(int argc, VALUE *argv, VALUE self){
    RUBY_TRY

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
        if (! NIL_P(hash_item)){
            policy.deadline = Rb_num_to_double(hash_item);
        }

        hash_item = rb_hash_aref(hash, rb_str_new2("timeout"));
        if (! NIL_P(hash_item)){
            policy.timeout = Rb_num_to_double(hash_item);
        }

        hash_item = rb_hash_aref(hash, rb_str_new2("max_retries"));
        if (! NIL_P(hash_item)){
            policy.max_retries = Rb_num_to_double(hash_item);
        }

        hash_item = rb_hash_aref(hash, rb_str_new2("urgent"));
        if (! NIL_P(hash_item)){
            policy.urgent = Rb_bool_to_bool(hash_item);
        }

        hash_item = rb_hash_aref(hash, rb_str_new2("persistent"));
        if (! NIL_P(hash_item)){
            policy.persistent = Rb_bool_to_bool(hash_item);
        }
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
    RUBY_CATCH
}

VALUE
dealer_manual_send(VALUE self, VALUE message_obj){
    RUBY_TRY

    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    message_holder* msg = get_ctype_pointer<message_holder>(message_obj);
    response_holder_t* resp = NULL; 
    try {
        int size = 0;
        resp = new response_holder_t(m_dealer->send_message(msg->get()));
    } catch(const dealer_error& e) {
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    } catch(const internal_error& e) {
            rb_raise(rb_eRuntimeError, e.what());
            return Qnil;
    }
    VALUE t_data = Data_Wrap_Struct(cResponse, 0, dispose<response_holder_t>, resp);
    return t_data;
    RUBY_CATCH
}

VALUE
dealer_get_stored_messages_count(VALUE self, VALUE service_alias){
/*
 * TODO:  Add try .. catch
 */
    RUBY_TRY
    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    size_t s = m_dealer->stored_messages_count(STR2CSTR(service_alias));
    return INT2FIX(s);
    RUBY_CATCH
}

VALUE
dealer_get_stored_messages(VALUE self, VALUE service_alias){
/*
 * TODO:  Add try .. catch
 */
    RUBY_TRY
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
    RUBY_CATCH
}

VALUE
dealer_remove_stored_message(VALUE self, VALUE message){
/*
 * TODO:  Add try .. catch
 */
    RUBY_TRY
    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    message_holder* msg = get_ctype_pointer<message_holder>(message);
    m_dealer->remove_stored_message(msg->get());
    return Qtrue; 
    RUBY_CATCH
}

VALUE
dealer_remove_stored_message_for(VALUE self, VALUE response){
    RUBY_TRY
    dealer_t* m_dealer = get_ctype_pointer<dealer_t>(self);
    response_holder_t* resp = get_ctype_pointer<response_holder_t>(response);
    m_dealer->remove_stored_message_for(resp->store_pointer());
    return Qtrue; 
    RUBY_CATCH
}

