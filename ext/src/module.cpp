#include "module.hpp"

/* Message class */
VALUE cMessage;

/* Response class */
VALUE cResponse;

/* Client class */
VALUE cClient;

extern "C"
void Init__dealer(){
    /* Dealer dealer class */
    cClient = rb_define_class("Client", rb_cObject);
    rb_define_singleton_method(cClient, "new", (VALUE(*)(...))&dealer_new, 1);
    rb_define_method(cClient, "send", (VALUE(*)(...))&dealer_send, -1);
    rb_define_method(cClient, "get_msg_count", (VALUE(*)(...))&dealer_get_stored_messages_count, 1);
    rb_define_method(cClient, "get_messages", (VALUE(*)(...))&dealer_get_stored_messages, 1);
    rb_define_method(cClient, "remove_stored_message", (VALUE(*)(...))&dealer_remove_stored_message, 1);
    /* Response class */
    cResponse = rb_define_class("Response", rb_cObject);
    rb_define_singleton_method(cResponse, "new", (VALUE(*)(...))&response_new, 0);
    rb_define_method(cResponse, "get", (VALUE(*)(...))&response_get, 1);
    /* Message_t class*/
    cMessage = rb_define_class("Message", rb_cObject);
}
