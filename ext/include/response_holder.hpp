#ifndef _RESPONSE_HOLDER_HPP
#define _RESPONSE_HOLDER_HPP

#include "cocaine/dealer/dealer.hpp"

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

        boost::shared_ptr<response_t> store_pointer(){
            return m_response;
        }
    private:
        boost::shared_ptr<response_t> m_response;
};

#endif
