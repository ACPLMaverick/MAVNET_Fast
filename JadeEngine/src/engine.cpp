#include "engine.h"

#include "mem/mem_manager.h"
#include "window/window.h"

namespace je {

    engine& engine::get_inst()
    {
        static engine s_engine;
        return s_engine;
    }

    void engine::run()
    {
        data::array<window::message> messages;

        while(m_is_exit == false)
        {
            // Process messages.
            const bool have_messages = m_window->poll_messages(messages);
            if(have_messages)
            {
                // TODO implement.
                messages.clear();
            }
            // TODO really implement.
#if JE_PLATFORM_LINUX
            timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 16'000'000;
            nanosleep(&ts, nullptr);
#endif // JE_PLATFORM_LINUX
        }
    }

    engine::engine()
        : m_is_exit(false)
        , m_mem_manager(new mem::mem_manager())
        , m_window(new window::window(1280, 720, false))    // TODO read these from config.
    {

    }

    engine::~engine()
    {
        JE_safe_delete(m_window);
        JE_safe_delete(m_mem_manager);
    }

}