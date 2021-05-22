#pragma once

#include "global.h"
#include "message.h"
#include "data/data.h"
#include "mem/allocatable.h"

#include "window/window_platform.h"

namespace je { namespace window {

#define JE_WINDOW_DEBUG_OUTPUT (1 && !(JE_CONFIG_RELEASE)) // TODO Implement

    class window : public mem::allocatable_persistent
    {
    public:

        // Creates a fullscreen borderless window on main display with the size of the main display.
        window();
        // Creates a window with a given size.
        window(u16 width, u16 height);
        ~window();

        void set_fullscreen(bool is_fullscreen);
        // When doing a manual resize, fullscreen will be automatically disabled.
        void resize(u16 new_width, u16 new_height);

        // Platform-specific.
        bool poll_messages(data::array<message>& out_messages);
        // ~Platform-specific.

        u16 get_width() const { return m_is_fullscreen ? m_display_width : m_width; }
        u16 get_height() const { return m_is_fullscreen ? m_display_height : m_height; }
        bool is_fullscreen() const { return m_is_fullscreen; }

    protected:

        void open();
        void close();

        // Platform-specific.
        void set_fullscreen_internal(bool is_fullscreen);
        void resize_internal(u16 width, u16 height);
        // ~Platform-specific.

        #include "window/window_platform.inl"

    protected:

        static const char* k_title;
        static const char* k_icon_path;

        u16 m_width;
        u16 m_height;
        u16 m_display_width;
        u16 m_display_height;
        bool m_is_fullscreen;
        bool m_is_open;
    };

}}