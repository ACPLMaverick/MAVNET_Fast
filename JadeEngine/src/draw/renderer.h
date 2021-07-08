#pragma once

#include "global.h"
#include "mem/allocatable.h"

namespace je { namespace window {

    class window;

}}

namespace je { namespace draw {

    class gpu;
    class presenter;

    class renderer : public mem::allocatable_persistent
    {
    public:

        renderer(window::window& win);
        ~renderer();

        void draw();

        inline bool is_gpu_created() const { return m_gpu != nullptr; }
        inline bool is_presenter_created() const { return m_presenter != nullptr; }

    protected:

        const window::window& m_window;
        gpu* m_gpu;
        presenter* m_presenter;

    };

}}