#pragma once

#include "global.h"

#include "mem/mem_manager.h"

namespace je {

    class engine
    {
    public:

        engine();
        ~engine();

        static engine& get_inst() { JE_assert(s_inst, "Null engine inst!"); return *s_inst; }

        mem::mem_manager& get_mem_manager() { return m_mem_manager; }

    private:

        inline void init_inst();
        inline void cleanup_inst();


        static engine* s_inst;

        mem::mem_manager m_mem_manager;
    };

}