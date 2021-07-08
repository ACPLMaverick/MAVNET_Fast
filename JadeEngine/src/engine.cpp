#include "engine.h"

#include "mem/mem_manager.h"
#include "window/window.h"
#include "draw/renderer.h"
#include "thread/thread.h"

namespace je {

    engine::engine()
        : m_is_exit(false)
    {
        mem::mem_manager::get_inst();   // TODO No control over when mem_manager gets cleaned up.
        m_window = new window::window(1280, 720); // TODO read these from config.
        m_renderer = new draw::renderer(get_window());
    }

    engine::~engine()
    {
        JE_safe_delete(m_renderer);
        JE_safe_delete(m_window);
    }

    void engine::run()
    {
        perform_post_init_checks();

        data::array<window::message> messages;

        while(m_is_exit == false)
        {
            // Process messages.
            const bool have_messages = m_window->poll_messages(messages);
            if(have_messages)
            {
                for(const window::message& msg : messages)
                {
                    if(msg.get_type() == window::message_type::k_closed)
                    {
                        // Shutdown engine and destroy the window.
                        m_is_exit = true;
                        break;  // No point in processing other messages.
                    }
                    else
                    {
                        // TODO Implement other messages.
                    }
                }
                messages.clear();
            }

            // Kick off scene update for next frame.
            // TODO

            // Draw scene for current frame, if available.
            m_renderer->draw();

            thread::thread::sleep_ms(16.67f);   // TODO sleep remaining of assumed frame time.
        }
    }

    void engine::perform_post_init_checks()
    {
        if(m_renderer->is_gpu_created() == false)
        {
            JE_fail("Fatal error. Could not create a GPU backend.");
            m_is_exit = true;
        }
        if(m_renderer->is_presenter_created() == false)
        {
            JE_fail("Fatal error. Could not create a swap chain.");
            m_is_exit = true;
        }
    }

}