#pragma once

#include "global.h"
#include "mem/allocatable.h"

namespace je { namespace window {

    class window;

}}

namespace je { namespace draw {

    namespace gpu {
        class device;
        class presenter;
    }

    class renderer : public mem::allocatable_persistent
    {
    public:

        renderer(window::window& win);
        ~renderer();

        void draw();

        inline bool is_gpu_backend_created() const { return m_device != nullptr && m_presenter != nullptr; }

    protected:

        const window::window& m_window;
        gpu::device* m_device;
        gpu::presenter* m_presenter;

    };

}}