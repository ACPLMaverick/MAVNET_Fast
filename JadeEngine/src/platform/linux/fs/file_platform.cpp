#include "fs/file.h"
#include "fs/system.h"
#include "math/sc.h"

namespace je { namespace fs {

    file::file()
        : m_size(0)
        , m_position(0)
#if JE_CONFIG_DEBUG
        , m_debug_path("")
#endif // JE_CONFIG_DEBUG
        , m_fd(-1)
    {

    }

    file::file(const data::string& a_path, options a_options)
#if JE_CONFIG_DEBUG
        : m_debug_path(a_path)
#endif // JE_CONFIG_DEBUG
    {
        JE_assert(system::is_mounted(), "File system is not mounted.");

        i32 flags = 0;
        if((static_cast<u8>(a_options) & (static_cast<u8>(options::k_read) | static_cast<u8>(options::k_write)))
            == (static_cast<u8>(options::k_read) | static_cast<u8>(options::k_write)))
        {
            flags = O_RDWR;
        }
        else if(static_cast<u8>(a_options) & static_cast<u8>(options::k_read))
        {
            flags = O_RDONLY;
        }
        else if(static_cast<u8>(a_options) & static_cast<u8>(options::k_write))
        {
            flags = O_WRONLY;
        }

        if(static_cast<u8>(a_options) & static_cast<u8>(options::k_append))
        {
            flags |= O_APPEND;
        }
        if(static_cast<u8>(a_options) & static_cast<u8>(options::k_create))
        {
            flags |= O_CREAT;
        }
        if(static_cast<u8>(a_options) & static_cast<u8>(options::k_hint_random_access))
        {
            // Not implemented in API?
            //flags |= O_RANDOM;
        }
        if(static_cast<u8>(a_options) & static_cast<u8>(options::k_hint_sequential_access))
        {
            // Not implemented in API?
            //flags |= O_SEQUENTIAL;
        }

        m_fd = open64(a_path.get_data(), flags);
        if(m_fd >= 0)
        {
            if(static_cast<u8>(a_options) & static_cast<u8>(options::k_clear))
            {
                // If this flag was specified, we should delete the file, and then re-create it.
                ::close(m_fd);
                if(unlink(a_path.get_data()) == 0)
                {
                    flags |= O_CREAT;
                    m_fd = open64(a_path.get_data(), flags);
                    JE_assert(m_fd >= 0, "Failed to recreate a file. [%s]", m_debug_path.get_data());
                }
            }
            struct stat64 sb = {};
            JE_verify(fstat64(m_fd, &sb) >= 0);
            m_size = sb.st_size;
            JE_assert(m_size > 0);
        }
        else
        {
            m_size = 0;
        }

        if(static_cast<u8>(a_options) & static_cast<u8>(options::k_append))
        {
            m_position = m_size;
        }
        else
        {
            m_position = 0;
        }
    }

    file::file(file&& a_other)
        : m_size(a_other.m_size)
        , m_position(a_other.m_position)
        , m_fd(a_other.m_fd)
    {
        a_other.m_size = 0;
        a_other.m_position = 0;
        a_other.m_fd = -1;
    }

    file::~file()
    {
        if(is_valid())
        {
            JE_assert(system::is_mounted(), "File system is not mounted.");
            ::close(m_fd);
        }
    }

    file& file::operator=(file&& a_other)
    {
        m_size = a_other.m_size;
        m_position = a_other.m_position;
        m_fd = a_other.m_fd;
        a_other.m_size = 0;
        a_other.m_position = 0;
        a_other.m_fd = -1;
        return *this;
    }

    bool file::read(size a_starting_position, size a_num_bytes, data_buffer& a_out_buffer, bool a_append_to_buffer)
    {
        if(is_valid() == false)
        {
            return false;
        }

        if(a_num_bytes == 0)
        {
            JE_fail("Cannot read 0 bytes from file.");
            return false;
        }

        set_position(a_starting_position);

        const size bytes_left = m_size - a_starting_position;
        if(bytes_left < a_num_bytes)
        {
            JE_fail("Wants to read [%zu] bytes from file but only [%zu] remains. Will clamp [%s].", a_num_bytes, bytes_left, m_debug_path.get_data());
            a_num_bytes = bytes_left;
        }

        u8* data_ptr = nullptr;
        if(a_append_to_buffer)
        {
            const size bytes_offset = a_out_buffer.size();
            a_out_buffer.resize(bytes_offset + a_num_bytes);
            data_ptr = a_out_buffer.data() + bytes_offset;
        }
        else
        {
            a_out_buffer.resize(a_num_bytes);
            data_ptr = a_out_buffer.data();
        }

        const size read_bytes = ::read(m_fd, data_ptr, a_num_bytes);

        JE_assert(read_bytes == a_num_bytes, "Failed to read all bytes from file [%s].", m_debug_path.get_data());
        return read_bytes == a_num_bytes;
    }

    bool file::write(const data_buffer& a_buffer, size a_starting_position)
    {
        if(is_valid() == false)
        {
            return false;
        }

        set_position(a_starting_position);
        const size bytes_to_write = a_buffer.size();
        if(bytes_to_write == 0)
        {
            JE_fail("Cannot write an empty buffer to file [%s].", m_debug_path.get_data());
            return false;
        }

        const size written_bytes = ::write(m_fd, a_buffer.data(), bytes_to_write);
        JE_assert(written_bytes == bytes_to_write, "Failed to write all bytes to file [%s].", m_debug_path.get_data());
        return written_bytes == bytes_to_write;
    }

    void file::set_position(size a_position)
    {
        if(is_valid() == false)
        {
            JE_fail("Trying to change position on an invalid file.");
            return;
        }

        JE_assert(a_position <= m_size, "Position set falls out of file boundary. Will be clamped. [%s].", m_debug_path.get_data());
        if(a_position == m_position)
        {
            return;
        }

        m_position = math::sc::min(a_position, m_size);
        lseek64(m_fd, m_position, SEEK_SET);
    }

    void file::move_position(i64 a_offset)
    {
        if(is_valid() == false)
        {
            JE_fail("Trying to change position on an invalid file.");
            return;
        }

        const i64 new_pos = (i64)(get_position()) + a_offset;
        JE_assert(new_pos >= 0 && new_pos <= m_size, "Move falls out of file boundary. Will be clamped. [%s].", m_debug_path.get_data());
        if(new_pos < 0)
        {
            a_offset -= (-new_pos);
        }
        else if(new_pos > (i64)m_size)
        {
            a_offset -= (new_pos - (i64)m_size);
        }
        lseek64(m_fd, a_offset, SEEK_CUR);
    }

}}