#pragma once

#include "global.h"
#include "fs/file_platform.h"
#include "data/string.h"

namespace je { namespace fs {

    using data_buffer = data::array<u8>;

    class file
    {
    public:

        enum class options : u8
        {
            k_read = (1 << 0),
            k_write = (1 << 1),
            k_clear = (1 << 2),
            k_append = (1 << 3),
            k_create = (1 << 4),
            k_hint_random_access = (1 << 5),
            k_hint_sequential_access = (1 << 6)
        };

        // TODO consider endianness.

    public:

        // Creates an invalid, empty file.
        file();
        // Path provided must be relative to the data directory and without engine-specific extension.
        file(const data::string& path, options a_options);
        file(file&& other);
        ~file();
        file& operator=(file&& other);

        size get_position() const { return m_position; }
        size get_size() const { return m_size; }
        bool is_valid() const { return get_size() > 0; }

        bool read_all(data_buffer& out_buffer, bool append_to_buffer = false);
        bool read(data_buffer& out_buffer, bool append_to_buffer = false);
        bool read(size num_bytes, data_buffer& out_buffer, bool append_to_buffer = false);
        bool read(size starting_position, size num_bytes, data_buffer& out_buffer, bool append_to_buffer = false);

        bool write_at_beginning(const data_buffer& buffer);
        bool write(const data_buffer& buffer);
        bool write(const data_buffer& buffer, size starting_position);

        void set_position(size position);
        void move_position(i64 offset);

    protected:

        size m_size;
        size m_position;
#if JE_CONFIG_DEBUG
        const data::string m_debug_path;
#endif // JE_CONFIG_DEBUG
        #include "fs/file_platform.inl"

    };

}}