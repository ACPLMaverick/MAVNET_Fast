#pragma once

#include "global.h"
#include "message.h"
#include "data/data.h"

namespace je { namespace window {

#define JE_WINDOW_DEBUG_OUTPUT (1 && !(JE_CONFIG_RELEASE))

    class window
    {
    public:

        window(u16 width, u16 height, bool is_fullscreen);
        ~window();

        bool poll_messages(data::array<message>& out_messages);
        void set_fullscreen(bool is_fullscreen);
        void resize(u16 new_width, u16 new_height);

        u16 get_width() const { return m_width; }
        u16 get_height() const { return m_height; }
        bool is_fullscreen() const { return m_is_fullscreen; }

    protected:

        void open();
        void close();

        static const char* k_title;
        static const char* k_icon_path;

        u64 m_display;
        u64 m_window;
        u16 m_width;
        u16 m_height;
        bool m_is_fullscreen;
        bool m_is_open;
    };

}}