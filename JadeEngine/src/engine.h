#pragma once

#include "global.h"

namespace je {

    namespace mem
    {
        class mem_manager;
    }

    class engine
    {
    public:

        static engine& get_inst();
        inline mem::mem_manager& get_mem_manager() { return JE_deref(m_mem_manager); }

    private:
        engine();
        ~engine();

        mem::mem_manager* m_mem_manager;
    };

}