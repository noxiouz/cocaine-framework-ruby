#ifndef _C_DEALER_HPP
#define _C_DEALER_HPP
#include "ruby.h"

VALUE dealer_new(VALUE cls, VALUE path);

VALUE
dealer_send(int argc, VALUE *argv, VALUE self);

VALUE 
dealer_manual_send(VALUE self, VALUE message_obj);

VALUE
dealer_get_stored_messages_count(VALUE self, VALUE service_alias);

VALUE
dealer_get_stored_messages(VALUE self, VALUE service_alias);

VALUE
dealer_remove_stored_message(VALUE self, VALUE message);

VALUE
dealer_remove_stored_message_for(VALUE self, VALUE response);
#endif
