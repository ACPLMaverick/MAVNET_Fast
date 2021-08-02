#include "pass.h"

namespace je { namespace draw { namespace gpu {

    void pass::set(cmd& a_cmd)
    {
        JE_todo();
    }

    void pass::unset(cmd& a_cmd)
    {
        JE_todo();
    }

    pass::pass(const pass_params& a_params)
        : m_params(a_params)
        , m_hash(m_params)
    {
    }

    pass::~pass()
    {
    }

}}}