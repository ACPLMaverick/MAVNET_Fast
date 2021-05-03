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

    class engine
    {
    public:

        static engine& get_inst();
        inline mem::mem_manager& get_mem_manager() { return JE_deref(m_mem_manager); }
        inline window::window& get_window() { return JE_deref(m_window); }

        void run();

    private:
        engine();
        ~engine();

        bool m_is_exit;
        mem::mem_manager* m_mem_manager;
        window::window* m_window;
    };

}