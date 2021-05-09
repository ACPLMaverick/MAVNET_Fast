#include "window.h"

namespace je { namespace window {

    window::window(u16 a_width, u16 a_height, bool a_is_fullscreen)
        : m_display(0)
        , m_window(0)
        , m_width(a_width)
        , m_height(a_height)
        , m_is_fullscreen(a_is_fullscreen)
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
        m_display = 0;
        m_window = 0;
        m_width = 0;
        m_height = 0;
        m_is_fullscreen = false;
        m_is_open = false;
    }

    const char* window::k_title = "JadeEngine";
    const char* window::k_icon_path = "icon.tga";

}}