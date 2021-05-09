#include "window/window.h"

#if JE_PLATFORM_LINUX

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

// ++TODO File loading.
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
// --TODO File loading.

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

    // Helpers.
    xcb_atom_t get_atom(xcb_connection_t* a_connection, const char* a_name)
    {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(a_connection, true, std::strlen(a_name), a_name);
        xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(a_connection, cookie, nullptr);
        JE_assert(reply != nullptr, "Could not retrieve an atom value.");
        xcb_atom_t atom = reply->atom;
        free(reply);
        return atom;
    }

    xcb_atom_t get_atom_value(xcb_connection_t* a_connection, xcb_window_t a_window, xcb_atom_t a_atom)
    {
        xcb_get_property_cookie_t cookie = xcb_get_property(a_connection, false, a_window, a_atom, XCB_ATOM_ATOM, 0, 32);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(a_connection, cookie, nullptr);
        JE_assert(reply != nullptr, "Could not get property value.");
        xcb_atom_t atom = *(reinterpret_cast<xcb_atom_t*>(xcb_get_property_value(reply)));
        free(reply);
        return atom;
    }

    void set_icon(xcb_connection_t* a_connection, xcb_window_t a_window, const char* a_icon_file_path)
    {
        // ++ TODO TEMP file loading until file system is implemented.
        const i32 fd = open64(a_icon_file_path, O_RDONLY);
        JE_assert(fd >= 0);
        struct stat64 sb = {};
        JE_verify(fstat64(fd, &sb) >= 0);
        const i32 length = sb.st_size;
        JE_assert(length > 0);

        u8* buffer = static_cast<u8*>(malloc(length));
        JE_verify(read(fd, buffer, length) == length);
        close(fd);
        // -- TODO TEMP file loading.

        // ++ TODO TEMP TGA Decoding until Textures are implemented.
        static const u64 tga_header_size = 18;
        static const u64 tga_width_offset = 8 + 4;
        static const u64 tga_height_offset = 8 + 6;
        static const u64 tga_pixel_depth_offset = 8 + 8;
        const u16 width = *reinterpret_cast<u16*>(buffer + tga_width_offset);
        const u16 height = *reinterpret_cast<u16*>(buffer + tga_height_offset);
        const u8 pixel_depth = buffer[tga_pixel_depth_offset];
        JE_assert(width == height, "An application icon must be square.");
        JE_assert(pixel_depth == 32, "An application icon must be in 32bit format.");

        static const u64 pixmap_header_size = 2 * sizeof(u32);
        const u32 bytes_to_offset_buffer = (tga_header_size - pixmap_header_size);
        const u32 siz = static_cast<u32>(static_cast<u64>(length) - bytes_to_offset_buffer);
        u8* buffer_with_offset = buffer + bytes_to_offset_buffer;

        *reinterpret_cast<u32*>(buffer_with_offset) = width;
        *(reinterpret_cast<u32*>(buffer_with_offset) + 1) = height;
        // -- TODO TEMP TGA Decoding.

        static xcb_atom_t s_atom_icon = get_atom(a_connection, "_NET_WM_ICON");
        static xcb_atom_t s_atom_cardinal = get_atom(a_connection, "CARDINAL");

        xcb_change_property (a_connection, XCB_PROP_MODE_REPLACE, a_window,
            s_atom_icon, s_atom_cardinal, 32,
            siz, buffer_with_offset);

        free(buffer);
    }
    // ~Helpers.

    bool window::poll_messages(data::array<message>& a_out_messages)
    {
        a_out_messages.clear();

        xcb_generic_event_t* generic_event(nullptr);
        xcb_connection_t* connection(load_platform_handle<xcb_connection_t*>(m_display));
        xcb_window_t window(load_platform_handle<xcb_window_t>(m_window));

        while((generic_event = xcb_poll_for_event(connection)) != nullptr)
        {
            switch (generic_event->response_type & ~0x80)
            {
            case XCB_VISIBILITY_NOTIFY:
            {
                xcb_visibility_notify_event_t* event = reinterpret_cast<xcb_visibility_notify_event_t*>(generic_event);
                if(event->window == window)
                {
                    if(event->state == XCB_VISIBILITY_UNOBSCURED)
                    {
                        a_out_messages.push_back(message(message_type::k_maximized));
                    }
                }
            }
                break;
            case XCB_RESIZE_REQUEST:
            {
                xcb_resize_request_event_t* event = reinterpret_cast<xcb_resize_request_event_t*>(generic_event);
                if(event->width != m_width || event->height != m_height)
                {
                    a_out_messages.push_back(message(message_type::k_resized, event->width, event->height));
                }
            }
                break;
            case XCB_FOCUS_IN:
            {
                a_out_messages.push_back(message(message_type::k_focus_gained));
            }
                break;
            case XCB_FOCUS_OUT:
            {
                a_out_messages.push_back(message(message_type::k_focus_lost));
            }
                break;
            case XCB_PROPERTY_NOTIFY:
            {
                xcb_property_notify_event_t* event = reinterpret_cast<xcb_property_notify_event_t*>(generic_event);
#if 0 // Not necessary to be called but leaving it here for now.
                xcb_get_atom_name_cookie_t atom_name_cookie = xcb_get_atom_name_unchecked(connection, event->atom);
                xcb_get_atom_name_reply_t* atom_name_reply = xcb_get_atom_name_reply(connection, atom_name_cookie, nullptr);
                if(atom_name_reply != nullptr)
                {
                    const char* atom_name = xcb_get_atom_name_name(atom_name_reply);
                    JE_print("%s", atom_name);
                    free(atom_name_reply);
                }
#endif // 0

                // Check for window minimization.
                static const xcb_atom_t k_atom_state = get_atom(connection, "_NET_WM_STATE");
                static const xcb_atom_t k_atom_state_hidden = get_atom(connection, "_NET_WM_STATE_HIDDEN");
                if(event->atom == k_atom_state)
                {
                    if(get_atom_value(connection, window, event->atom) == k_atom_state_hidden)
                    {
                        a_out_messages.push_back(message(message_type::k_minimized));
                    }
                }
            }
                break;
            case XCB_CLIENT_MESSAGE:
            {
                // Close window.
                xcb_client_message_event_t* event = reinterpret_cast<xcb_client_message_event_t*>(generic_event);
                if(event->data.data32[0] == static_cast<xcb_atom_t>(m_atom_close))
                {
                    a_out_messages.push_back(message(message_type::k_closed));
                }
            }
                break;
            default:
                JE_print("[Window] Unknown event [%d].", generic_event->response_type);
                break;
            }

            free(generic_event);
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
            XCB_EVENT_MASK_VISIBILITY_CHANGE
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
            strlen(k_title), k_title);

        // Acquire the close atom.
        // https://stackoverflow.com/questions/47453159/xcb-poll-for-event-does-not-detect-xcb-client-message-event-for-closing-window
        xcb_intern_atom_cookie_t protocol_cookie = xcb_intern_atom_unchecked(connection, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* protocol_reply = xcb_intern_atom_reply(connection, protocol_cookie, 0);
        xcb_intern_atom_cookie_t close_cookie = xcb_intern_atom_unchecked(connection, 0, 16, "WM_DELETE_WINDOW");
        m_atom_close = static_cast<u32>(xcb_intern_atom_reply(connection, close_cookie, 0)->atom);
        xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, protocol_reply->atom, 4, 32, 1, static_cast<xcb_atom_t*>(&m_atom_close));
        free(protocol_reply);

        set_icon(connection, window, k_icon_path);

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