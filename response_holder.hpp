
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
    private:
        boost::shared_ptr<response_t> m_response;
};
