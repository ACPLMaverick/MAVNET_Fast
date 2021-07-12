#pragma once

#include "global.h"
#include "mem/allocatable.h"

namespace je { namespace window {

    class window;

}}

namespace je { namespace draw {

    namespace gpu {
        class dev;
        class presenter;
    }

    class renderer : public mem::allocatable_persistent
    {
    public:

        renderer(window::window& win);
        ~renderer();

        void draw();

        inline bool is_gpu_backend_created() const { return m_dev != nullptr && m_presenter != nullptr; }

    protected:

        const window::window& m_window;
        gpu::dev* m_dev;
        gpu::presenter* m_presenter;

    };

}}