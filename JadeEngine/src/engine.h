#pragma once

#include "global.h"

namespace je {

    namespace mem
    {
        class mem_manager;
    }
    namespace window
    {
        class window;
    }
    namespace draw
    {
        class renderer;
    }
    

    class engine
    {
    public:

        engine();
        ~engine();

        void run();

        inline window::window& get_window() { return JE_deref(m_window); }
        inline draw::renderer& get_renderer() { return JE_deref(m_renderer); }

    private:

        void perform_post_init_checks();

    private:

        bool m_is_exit;
        window::window* m_window;
        draw::renderer* m_renderer;
    };

}