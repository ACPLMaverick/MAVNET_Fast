#include "file.h"
#include "math/sc.h"

namespace je { namespace fs {

    bool file::read_all(data_buffer& a_out_buffer, bool a_append_to_buffer)
    {
        if(is_valid() == false)
        {
            return false;
        }
        set_position(0);
        return read(get_position(), get_size(), a_out_buffer, a_append_to_buffer);
    }

    bool file::read(data_buffer& a_out_buffer, bool a_append_to_buffer)
    {
        if(is_valid() == false)
        {
            return false;
        }
        return read(get_position(), get_size(), a_out_buffer, a_append_to_buffer);
    }

    bool file::read(size a_num_bytes, data_buffer& a_out_buffer, bool a_append_to_buffer)
    {
        if(is_valid() == false)
        {
            return false;
        }
        return read(get_position(), a_num_bytes, a_out_buffer, a_append_to_buffer);
    }

    bool file::write_at_beginning(const data_buffer& buffer)
    {
        if(is_valid() == false)
        {
            return false;
        }
        set_position(0);
        return write(buffer, get_position());
    }
    
    bool file::write(const data_buffer& buffer)
    {
        if(is_valid() == false)
        {
            return false;
        }
        return write(buffer, get_position());
    }

}}