#include "window.h"

namespace je { namespace window {

    window::window()
        : m_width(0)
        , m_height(0)
        , m_display_width(0)
        , m_display_height(0)
        , m_is_fullscreen(true)
        , m_is_open(false)
    {
        open();
    }

    window::window(u16 a_width, u16 a_height)
        : m_width(a_width)
        , m_height(a_height)
        , m_display_width(0)
        , m_display_height(0)
        , m_is_fullscreen(false)
        , m_is_open(false)
    {
        open();
    }

    window::~window()
    {
        if(m_is_open)
        {
            close();
        }
        m_width = 0;
        m_height = 0;
        m_is_fullscreen = false;
        m_is_open = false;
    }

    void window::set_fullscreen(bool a_is_fullscreen)
    {
        if(m_is_fullscreen == a_is_fullscreen)
        {
            return;
        }

        m_is_fullscreen = a_is_fullscreen;
        
        if(m_is_fullscreen)
        {
            set_fullscreen_internal(true);
            resize_internal(m_display_width, m_display_height);
        }
        else
        {
            set_fullscreen_internal(false);
            resize_internal(m_width, m_height);
        }
    }

    void window::resize(u16 a_new_width, u16 a_new_height)
    {
        if(get_width() == a_new_width || get_height() == a_new_height)
        {
            return;
        }

        m_width = a_new_width;
        m_height = a_new_height;

        if(is_fullscreen())
        {
            set_fullscreen(false);  // This will call resize_internal.
        }
        else
        {
            resize_internal(m_width, m_height);
        }
    }

    const char* window::k_title = "JadeEngine";
    const char* window::k_icon_path = "icon.tga";

}}