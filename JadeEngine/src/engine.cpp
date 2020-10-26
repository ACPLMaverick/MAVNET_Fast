#include "engine.h"

#include "mem/mem_manager.h"

namespace je {

    engine& engine::get_inst()
    {
        static engine s_engine;
        return s_engine;
    }

    engine::engine()
        : m_mem_manager(new mem::mem_manager())
    {

    }

    engine::~engine()
    {
        JE_safe_delete(m_mem_manager);
    }

}