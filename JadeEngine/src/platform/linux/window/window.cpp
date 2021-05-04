#include "window/window.h"

#if JE_PLATFORM_LINUX

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

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
        /*
        XCB_EVENT_MASK_EXPOSURE
        XCB_EVENT_MASK_VISIBILITY_CHANGE
        XCB_EVENT_MASK_RESIZE_REDIRECT
        XCB_EVENT_MASK_FOCUS_CHANGE
        XCB_EVENT_MASK_PROPERTY_CHANGE
        */
        a_out_messages.clear();

        xcb_generic_event_t* event(nullptr);
        xcb_connection_t* connection(load_platform_handle<xcb_connection_t*>(m_display));
        while((event = xcb_poll_for_event(connection)) != nullptr)
        {
            switch (event->response_type)
            {
            case XCB_EXPOSE:
                /* code */
                break;
            case XCB_VISIBILITY_NOTIFY:
                break;
            case XCB_RESIZE_REQUEST:
                break;
            case XCB_FOCUS_IN:
                break;
            case XCB_FOCUS_OUT:
                break;
            case XCB_PROPERTY_NOTIFY:
                break;
            default:
                break;
            }

            free(event);
        }

        return a_out_messages.size() > 0;
    }

    void window::set_fullscreen(bool a_is_fullscreen)
    {
        if(m_is_fullscreen == a_is_fullscreen)
        {
            return;
        }

        JE_todo();
    }

    void window::resize(u16 a_new_width, u16 a_new_height)
    {
        if(get_width() == a_new_width || get_height() == a_new_height)
        {
            return;
        }

        xcb_connection_t* connection(load_platform_handle<xcb_connection_t*>(m_display));
        xcb_window_t window(load_platform_handle<xcb_window_t>(m_window));

        const i32 dims[] = { static_cast<i32>(a_new_width), static_cast<i32>(a_new_height) };
        xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, dims);
        xcb_flush(connection);

        m_width = a_new_width;
        m_height = a_new_height;
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

        // Define event mask.
        const u32 mask(XCB_CW_EVENT_MASK);
        const u32 events
        (
            XCB_EVENT_MASK_EXPOSURE
            | XCB_EVENT_MASK_VISIBILITY_CHANGE
            | XCB_EVENT_MASK_RESIZE_REDIRECT
            | XCB_EVENT_MASK_FOCUS_CHANGE
            | XCB_EVENT_MASK_PROPERTY_CHANGE
        );

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
            mask,
            &events
        );
        xcb_map_window(connection, window);

        /* For some WMs the X/Y coordinates are not taken into account
        when passed to xcb_create_window. As a workaround we must
        manually set the coordinates after mapping the window. */
        const i32 coords[] = { static_cast<i32>(pos_x), static_cast<i32>(pos_y) };
        xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);

        /* Set the title of the window */
        xcb_change_property (connection, XCB_PROP_MODE_REPLACE, window,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
            strlen (k_title), k_title);

        /* Set the title of the window icon */
        xcb_change_property (connection, XCB_PROP_MODE_REPLACE, window,
            XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8,
            strlen(k_icon_path), k_icon_path);

        xcb_flush(connection);

        m_is_open = true;
    }

    void window::close()
    {
        xcb_connection_t* connection(load_platform_handle<xcb_connection_t*>(m_display));
        xcb_destroy_window(connection, load_platform_handle<xcb_window_t>(m_window));
        xcb_disconnect(connection);
        m_is_open = false;
    }

}}

#endif // JE_PLATFORM_LINUX