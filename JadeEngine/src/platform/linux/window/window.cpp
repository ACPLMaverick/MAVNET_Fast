#include "window/window.h"

#if JE_PLATFORM_LINUX

#include <xcb/xcb.h>

// TODO HELPER Move elsewhere.
namespace je {
    template<typename stored_type, typename storage_type>
    inline void store_platform_handle(storage_type& storage, stored_type handle)
    {
        static_assert(sizeof(storage_type) >= sizeof(stored_type), "Platform handle does not fit in storage type.");
        reinterpret_cast<stored_type&>(storage) = handle;
    }

    template<typename stored_type, typename storage_type>
    inline stored_type load_platform_handle(const storage_type& storage)
    {
        static_assert(sizeof(storage_type) >= sizeof(stored_type), "Platform handle does not fit in storage type.");
        return (stored_type)(storage);  // Old-style cast because cannot always differentiate between reinterpret and static cast.
    }
}
// ~TODO

namespace je { namespace window {

    bool window::poll_messages(data::array<message>& a_out_messages)
    {
        return false;
    }

    void window::set_fullscreen(bool a_is_fullscreen)
    {
        JE_todo();
    }

    void window::resize(u16 a_new_width, u16 a_new_height)
    {
        JE_todo();
    }

    void window::open()
    {
        JE_assert_bailout_noret((m_width > 0 && m_height > 0) || m_is_fullscreen, "Invalid window parameters.");

        // Open connection.
        i32 screen_number(0);
        xcb_connection_t* connection(xcb_connect(nullptr, &screen_number));
        JE_assert_bailout_noret(connection != nullptr, "Could not create XCB connection.");
        store_platform_handle(m_display, connection);

        // Get first available screen. TODO - store this in config - info what screen was the window on
        // the last time it was used. And recreate it on this screen.
        xcb_screen_t* screen(nullptr);
        for(xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection)); iter.rem; --screen_number, xcb_screen_next(&iter))
        {
            if(screen_number == 0)
            {
                screen = iter.data;
                break;
            }
        }

        JE_assert_bailout_noret(screen != nullptr, "Could not find an appropriate screen.");

        // Create a window.
        // TODO - store window position in config as well to restore in the same place.
        i16 pos_x = 0;
        i16 pos_y = 0;
        u16 width = screen->width_in_pixels;
        u16 height = screen->height_in_pixels;

        // Check if we can/should create non-fullscreen window. Always create at the center of the screen
        if(m_is_fullscreen == false
            && screen->width_in_pixels > m_width
            && screen->height_in_pixels > m_height)
        {
            // If so, update parameters.
#if 0   // Skip position offsetting until I figure out how to do it properly.
        // https://stackoverflow.com/questions/36966900/xcb-get-all-monitors-ands-their-x-y-coordinates
            pos_x = screen->width_in_pixels / 2 - m_width / 2;
            pos_y = screen->height_in_pixels / 2 - m_height / 2;
#endif // 0
            width = m_width;
            height = m_height;
        }

        // For now ignore the fullscreen. TODO - Draw fullscreen borderless if fulscreen specified and ignore m_width / m_height.

        xcb_window_t window(xcb_generate_id(connection));
        JE_assert_bailout_noret(window > 0, "Could not create window ID");
        store_platform_handle(m_window, window);

        xcb_create_window
        (
            connection,
            XCB_COPY_FROM_PARENT,
            window,
            screen->root,
            pos_x,
            pos_y,
            width,
            height,
            0, // No border.
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual,
            0, NULL
        );
        xcb_map_window(connection, window);

        /* For some WMs the X/Y coordinates are not taken into account
        when passed to xcb_create_window. As a workaround we must
        manually set the coordinates after mapping the window. */
        const i32 coords[] = { static_cast<i32>(pos_x), static_cast<i32>(pos_y) };
        xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);

        xcb_flush(connection);
    }

    void window::close()
    {
        xcb_connection_t* connection(load_platform_handle<xcb_connection_t*>(m_display));
        xcb_destroy_window(connection, load_platform_handle<xcb_window_t>(m_window));
        xcb_disconnect(connection);
    }

}}

#endif // JE_PLATFORM_LINUX