#ifndef C_MESSAGE
#define C_MESSAGE

#include "cocaine/dealer/message.hpp"

class message_holder {
    public:
        explicit message_holder(message_t& _msg):msg(_msg){}
        message_t get(){return msg;}
    private:
        message_t msg;
};



#endif
