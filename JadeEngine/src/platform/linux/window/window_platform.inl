#if JE_PLATFORM_LINUX

// namespace je { namespace window {

    // class window 
    // {

            xcb_atom_t get_atom(const char* name);
            xcb_atom_t get_atom_property_value(xcb_atom_t atom);
            void set_icon();
            void get_display_dimensions(xcb_screen_t* a_screen, u16& a_out_width, u16& a_out_height);

            xcb_connection_t* m_connection{nullptr};
            xcb_window_t m_window{0};
            xcb_atom_t m_atom_close{0};

    // };

//}}

#endif // JE_PLATFORM_LINUX