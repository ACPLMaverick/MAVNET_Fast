#include "message.h"

namespace je { namespace window {

    message::message(message_type a_type, u16 a_param_a/* = 0*/, u16 a_param_b/* = 0*/)
        : m_param_a(a_param_a)
        , m_param_b(a_param_b)
        , m_type(a_type)
    {
    }

    message::~message()
    {
    }

}}