#include "engine.h"

namespace je {

    engine::engine()
        : m_mem_manager()
    {
        init_inst();
    }

    engine::~engine()
    {
        cleanup_inst();
    }

    void engine::init_inst()
    {
        if(s_inst == nullptr)
        {
            s_inst = this;
        }
        else
        {
            JE_fail("Cannot have more than one instance of the engine.");
        }
    }

    void engine::cleanup_inst()
    {
        if(s_inst != nullptr)
        {
            s_inst = nullptr;
        }
        else
        {
            JE_fail("Destructor of engine encountered an empty inst.");
        }
    }

    engine* engine::s_inst(nullptr);

}