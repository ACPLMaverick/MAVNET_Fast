#include "string.h"

#include <cstring>
#include <stdarg.h>

namespace je { namespace data {

#define JE_check_str_bailout_ret(_str_, _ret_val_) if(_str_ == nullptr || _str_[0] == char_end) return _ret_val_
#define JE_check_str_bailout(_str_) if(_str_ == nullptr || _str_[0] == char_end) return

    string::hash::hash(const char_type* a_str)
    {
        build(a_str);
    }

    string::hash::hash(const hash& a_other)
        : m_value(a_other.m_value)
    {

    }

    string::hash::hash(hash&& a_move)
        : m_value(a_move.m_value)
    {
        a_move.m_value = 0;
    }

    string::hash& string::hash::operator=(const hash& a_other)
    {
        m_value = a_other.m_value;
        return *this;
    }

    string::hash& string::hash::operator=(hash&& a_move)
    {
        m_value = a_move.m_value;
        a_move.m_value = 0;
        return *this;
    }

    void string::hash::build(const char_type* a_str)
    {
        if(a_str != nullptr)
        {
            // http://www.cse.yorku.ca/~oz/hash.html - djb2.
            m_value = 5381;
            uint32_t c = 0;
            while ((c = *(a_str++)) != 0)
                m_value = ((m_value << 5) + m_value) + c; /* hash * 33 + c */
        }
        else
        {
            m_value = 0;
        }
    }

    string::string()
        : m_chars(char_end)
        , m_hash(get_data())
    {
    }

    string::string(const char_type* a_str)
        : m_hash(nullptr)
    {
        create_from_str(a_str);
    }

    string::string(size_t a_num_chars_to_have)
        : m_chars()
        , m_hash(nullptr)
    {
        m_chars.resize(a_num_chars_to_have);
    }
    
    string::string(const string& a_copy)
        : m_chars(a_copy.m_chars)
        , m_hash(a_copy.m_hash)
    {
    }
    
    string::string(string&& a_move)
        : m_chars(std::move(a_move.m_chars))
        , m_hash(a_move.m_hash)
    {
    }
    
    string::~string()
    {
    }
    
    string& string::operator=(const char_type* a_str)
    {
        create_from_str(a_str);
        return *this;
    }
    
    string& string::operator=(const string& a_copy)
    {
        m_chars = a_copy.m_chars;
        m_hash = a_copy.m_hash;
        return *this;
    }
    
    string& string::operator=(const string&& a_move)
    {
        m_chars = std::move(a_move.m_chars);
        m_hash = std::move(a_move.m_hash);
        return *this;
    }
    

    string string::format(const char_type* a_format, ...)
    {
        va_list args;
        va_start(args, a_format);

        const int char_num = vsnprintf(nullptr, 0, a_format, args);

        JE_assert(char_num >= 0, "Failed to format a string.");
        if(char_num <= 0)
        {
            va_end(args);
            return string(nullptr);
        }
        else
        {
            string str(char_num + 1);
            vsprintf_s(str.m_chars.data(), char_num + 1, a_format, args);
            va_end(args);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_int64(int64_t a_value)
    {
        static const char_type* k_fmt = "%lld";
        const int char_num = std::snprintf(nullptr, 0, k_fmt, a_value);
        if(char_num <= 0)
        {
            JE_fail("Failed to parse a number [%lld].", a_value);
            return string(nullptr);
        }
        else
        {
            string str(char_num + 1);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_uint64(uint64_t a_value)
    {
        static const char_type* k_fmt = "%llu";
        const int char_num = std::snprintf(nullptr, 0, k_fmt, a_value);
        if(char_num <= 0)
        {
            JE_fail("Failed to parse a number [%llu].", a_value);
            return string(nullptr);
        }
        else
        {
            string str(char_num + 1);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_int32(int32_t a_value)
    {
        static const char_type* k_fmt = "%d";
        const int char_num = std::snprintf(nullptr, 0, k_fmt, a_value);
        if(char_num <= 0)
        {
            JE_fail("Failed to parse a number [%d].", a_value);
            return string(nullptr);
        }
        else
        {
            string str(char_num + 1);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_uint32(uint32_t a_value)
    {
        static const char_type* k_fmt = "%u";
        const int char_num = std::snprintf(nullptr, 0, k_fmt, a_value);
        if(char_num <= 0)
        {
            JE_fail("Failed to parse a number [%u].", a_value);
            return string(nullptr);
        }
        else
        {
            string str(char_num + 1);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_float(float a_value, size_t a_decimal_places/* = std::numeric_limits<size_t>::max()*/)
    {
        static const char_type* k_fmt_base = "%f";
        static const size_t k_fmt_dec_buf_size = 6;
        static thread_local char_type k_fmt_dec_buf[k_fmt_dec_buf_size] = {};

        const char_type* fmt = k_fmt_base;
        if(a_decimal_places != std::numeric_limits<size_t>::max())
        {
            static const char_type* k_fmt_dec = ".%d";
            const int num_chars = std::snprintf(k_fmt_dec_buf + 1, k_fmt_dec_buf_size - 3, k_fmt_dec, a_decimal_places);
            k_fmt_dec_buf[0] = '%';
            k_fmt_dec_buf[1 + num_chars] = 'f';
            k_fmt_dec_buf[1 + num_chars + 1] = 0;
            fmt = k_fmt_dec_buf;
        }

        const int char_num = std::snprintf(nullptr, 0, fmt, a_value);
        if(char_num <= 0)
        {
            JE_fail("Failed to parse a number [%f].", a_value);
            return string(nullptr);
        }
        else
        {
            string str(char_num + 1);
            std::snprintf(str.m_chars.data(), char_num + 1, fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }

    int64_t string::parse_int64(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        int64_t val = std::strtoll(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    uint64_t string::parse_uint64(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        uint64_t val = std::strtoull(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    int32_t string::parse_int32(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        int32_t val = std::strtol(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    uint32_t string::parse_uint32(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        uint32_t val = std::strtoul(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    float string::parse_float(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        float val = std::strtof(a_str, nullptr);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    int64_t string::parse_int64(const string& a_str)
    {
        return parse_int64(a_str.get_data());
    }

    uint64_t string::parse_uint64(const string& a_str)
    {
        return parse_uint64(a_str.get_data());
    }

    int32_t string::parse_int32(const string& a_str)
    {
        return parse_int32(a_str.get_data());
    }

    uint32_t string::parse_uint32(const string& a_str)
    {
        return parse_uint32(a_str.get_data());
    }

    float string::parse_float(const string& a_str)
    {
        return parse_float(a_str.get_data());
    }

    void string::clear()
    {
        m_chars.clear();
        m_chars.push_back(char_end);
        chars_have_changed();
    }
    
    void string::resize(size_t a_num_chars, char_type a_char_to_fill/* = char_end*/)
    {
        m_chars.resize(a_num_chars + 1);
        std::memset(m_chars.data(), static_cast<int>(a_char_to_fill), a_num_chars * sizeof(char_type));
        m_chars[m_chars.size() - 1] = char_end;
        chars_have_changed();
    }
    
    void string::reserve(size_t a_num_chars)
    {
        m_chars.reserve(a_num_chars + 1);
    }
    

    size_t string::get_size() const
    {
        // TODO: Better idea to use strlen for this?
        JE_assert(m_chars.size() > 0, "Bad chars array size.");
        return m_chars.size() - 1;
    }
    
    bool string::is_empty() const
    {
        return get_size() == 0;
    }
    
    bool string::is_starting_with(const char_type* a_str) const
    {
        JE_check_str_bailout_ret(a_str, false);

        const size_t my_size = get_size();
        size_t i = 0;
        for(/**/; a_str[i] != char_end && i < my_size; ++i)
        {
            const char_type my_char = m_chars[i];
            const char_type other_char = a_str[i];
            if(my_char != other_char)
            {
                return false;
            }
        }

        // If the iterator has exceeded MY string, it means that OTHER string has the same beginning but it's longer.
        // Return false in this case.
        return i < my_size || (i == my_size && a_str[i] == char_end);
    }
    
    bool string::is_starting_with(const string& a_str) const
    {
        return is_starting_with(a_str.get_data());
    }
    
    bool string::is_ending_with(const char_type* a_str) const
    {
        JE_check_str_bailout_ret(a_str, false);

        const size_t other_size = std::strlen(a_str);
        const size_t my_size = get_size();

        size_t io = other_size - 1;
        size_t im = my_size - 1;
        for(/**/; io < other_size && im < my_size; --io, --im)
        {
            const char_type my_char = m_chars[im];
            const char_type other_char = a_str[io];
            if(my_char != other_char)
            {
                return false;
            }
        }

        // If the iterator has reached 0 MY string, it means that OTHER string has the same ending but it's longer.
        // Return false in this case.
        return im < my_size || (im == std::numeric_limits<size_t>::max() && io == std::numeric_limits<size_t>::max());
    }
    
    bool string::is_ending_with(const string& a_str) const
    {
        return is_ending_with(a_str.get_data());
    }
    

    bool string::operator==(const char_type* a_str) const
    {
        // TODO What is faster - strcmp or computing hash?
        hash str_hash(a_str);
        return m_hash == str_hash;
    }
    
    bool string::operator==(const string& a_str) const
    {
        return m_hash == a_str.m_hash;
    }
    

    string::char_type string::operator[](size_t a_index) const
    {
        JE_assert(a_index < get_size(), "Invalid index.");
        return m_chars[a_index];
    }
    
    const string::char_type* string::get_data() const
    {
        return &m_chars.front();
    }
    
    const string::char_type* string::operator*() const
    {
        return get_data();
    }
    

    void string::append(const char_type* a_str)
    {
        JE_check_str_bailout(a_str);
        const size_t num_chars = std::strlen(a_str);
        append_common(a_str, num_chars);
    }
    
    void string::append(const string& a_str)
    {
        if(a_str.is_empty())
        {
            return;
        }

        append_common(a_str.get_data(), a_str.get_size());
    }
    
    void string::operator+=(const char_type* a_str)
    {
        append(a_str);
    }
    
    void string::operator+=(const string& a_str)
    {
        append(a_str);
    }
    
    string string::operator+(const char_type* a_str)
    {
        JE_check_str_bailout_ret(a_str, string(*this));
        string str(*this);
        str.append(a_str);
        return a_str;
    }
    
    string string::operator+(const string& a_str)
    {
        string str(*this);
        str.append(a_str);
        return a_str;
    }
    
    
    void string::insert(const char_type* a_str, size_t a_idx_dest/* = invalid_idx*/,
        size_t a_idx_src_start/* = invalid_idx*/, size_t a_idx_src_end/* = invalid_idx*/)
    {
        JE_check_str_bailout(a_str);
        const size_t num_chars = std::strlen(a_str);
        insert_common(a_str, num_chars, a_idx_dest, a_idx_src_start, a_idx_src_end);
    }
    
    void string::insert(const string& a_str, size_t a_idx_dest/* = invalid_idx*/,
        size_t a_idx_src_start/* = invalid_idx*/, size_t a_idx_src_end/* = invalid_idx*/)
    {
        insert_common(a_str.get_data(), a_str.get_size(), a_idx_dest, a_idx_src_start, a_idx_src_end);
    }
    
    void string::replace(const char_type* a_str, size_t a_idx_dest_start/* = invalid_idx*/,
        size_t a_idx_dest_end/* = invalid_idx*/, size_t a_idx_src_start/* = invalid_idx*/,
        size_t a_idx_src_end/* = invalid_idx*/)
    {
        JE_check_str_bailout(a_str);
        const size_t num_chars = std::strlen(a_str);
        replace_common(a_str, num_chars, a_idx_dest_start, a_idx_dest_end, a_idx_src_start, a_idx_src_end);
        chars_have_changed();
    }
    
    void string::replace(const string& a_str, size_t a_idx_dest_start/* = invalid_idx*/,
        size_t a_idx_dest_end/* = invalid_idx*/,
        size_t a_idx_src_start/* = invalid_idx*/, size_t a_idx_src_end/* = invalid_idx*/)
    {
        replace_common(a_str.get_data(), a_str.get_size(), a_idx_dest_start, a_idx_dest_end, a_idx_src_start, a_idx_src_end);
        chars_have_changed();
    }


    bool string::find_and_replace(const char_type* a_str_to_find, const char_type* a_str_to_replace_it_with)
    {
        JE_check_str_bailout_ret(a_str_to_find, false);
        JE_check_str_bailout_ret(a_str_to_replace_it_with, false);
        const size_t num_chars_to_find = std::strlen(a_str_to_find);
        const size_t num_chars_to_replace = std::strlen(a_str_to_replace_it_with);
        return find_and_replace_common(a_str_to_find, num_chars_to_find, a_str_to_replace_it_with, num_chars_to_replace);
    }

    bool string::find_and_replace(const string& a_str_to_find, const string& a_str_to_replace_it_with)
    {
        return find_and_replace_common(a_str_to_find.get_data(), a_str_to_find.get_size(), a_str_to_replace_it_with.get_data(), a_str_to_replace_it_with.get_size());
    }

    bool string::find_and_replace(const string& a_str_to_find, const char_type* a_str_to_replace_it_with)
    {
        JE_check_str_bailout_ret(a_str_to_replace_it_with, false);
        const size_t num_chars_to_replace = std::strlen(a_str_to_replace_it_with);
        return find_and_replace_common(a_str_to_find.get_data(), a_str_to_find.get_size(), a_str_to_replace_it_with, num_chars_to_replace);
    }

    bool string::find_and_replace(const char_type* a_str_to_find, const string& a_str_to_replace_it_with)
    {
        JE_check_str_bailout_ret(a_str_to_find, false);
        const size_t num_chars_to_find = std::strlen(a_str_to_find);
        return find_and_replace_common(a_str_to_find, num_chars_to_find, a_str_to_replace_it_with.get_data(), a_str_to_replace_it_with.get_size());
    }
    
    
    void string::swap(string& a_str)
    {
        JE_todo();
    }

    
    void string::substring(size_t a_idx_start, size_t a_idx_end)
    {
        JE_todo();
    }
    
    void string::trim_front(size_t a_num_chars)
    {
        JE_todo();
    }
    
    void string::trim_end(size_t a_num_chars)
    {
        JE_todo();
    }
    
    void string::split(const char_type* a_split_on, array<string> a_out_strings) const
    {
        JE_todo();
    }
    
    void string::split(const string& a_split_on, array<string> a_out_strings) const
    {
        JE_todo();
    }
    

    size_t string::find(const char_type* a_str) const
    {
        JE_check_str_bailout_ret(a_str, invalid_idx);
        const size_t num_chars = std::strlen(a_str);
        return find_common(a_str, num_chars);
    }
    
    size_t string::find(const string& a_str) const
    {
        return find_common(a_str.get_data(), a_str.get_size());
    }
    
    bool string::contains(const char_type* a_str) const
    {
        JE_todo();
        return false;
    }
    
    bool string::contains(const string& a_str) const
    {
        JE_todo();
        return false;
    }
    

    void string::to_upper_case()
    {
        JE_todo();
    }
    
    void string::to_lower_case()
    {
        JE_todo();
    }
    
    void string::to_capitalized_case()
    {
        JE_todo();
    }
    
    bool string::is_upper_case() const
    {
        JE_todo();
        return false;
    }
    
    bool string::is_lower_case() const
    {
        JE_todo();
        return false;
    }
    

    inline void string::append_common(const char_type* a_str, size_t a_num_chars)
    {
        const size_t prev_zero_idx = get_size();
        m_chars.resize(m_chars.size() + a_num_chars);
        std::memcpy(&m_chars[prev_zero_idx], a_str, a_num_chars * sizeof(char_type));
        m_chars[m_chars.size() - 1] = char_end;

        chars_have_changed();
    }

    inline void string::create_from_str(const char_type* a_str)
    {
        if(a_str == nullptr)
        {
            m_chars.push_back(char_end);
            return;
        }

        // TODO Check what is faster: strlen and memcpy or adding chars one by one.
        /*
        do
        {
            m_chars.push_back(*a_str);
        } while(*(++a_str) != char_end);
        */
        const int len = std::strlen(a_str);
        if(len > 0)
        {
            m_chars.resize(len);
            std::memcpy(m_chars.data(), a_str, len * sizeof(char_type));
        }
        m_chars.push_back(char_end);

        chars_have_changed();
    }

    inline void string::insert_common(const char_type* a_str, size_t a_num_chars,
        size_t a_idx_dest, size_t a_idx_src_start, size_t a_idx_src_end)
    {
        const size_t my_size = get_size();
        if(a_idx_dest == invalid_idx)
        {
            a_idx_dest = my_size;
        }
        if(a_idx_src_start == invalid_idx)
        {
            a_idx_src_start = 0;
        }
        if(a_idx_src_end == invalid_idx)
        {
            a_idx_src_end = a_num_chars - 1;
        }
        JE_assert(a_idx_dest <= my_size
            && a_idx_src_start < a_num_chars
            && a_idx_src_end < a_num_chars
            && a_idx_src_start < a_idx_src_end,
            "Invalid indices for string operation.");

        const size_t num_chars_to_insert = a_idx_src_end - a_idx_src_start + 1;

        const size_t m_prev_size_with_end_char = m_chars.size();
        m_chars.resize(m_chars.size() + num_chars_to_insert);
        char_type* place_to_insert = &m_chars[a_idx_dest];
        if(a_idx_dest != my_size)
        {
            // Want to insert chars in the middle of the string - need to move the remaining part first.
            char_type* dst = place_to_insert + num_chars_to_insert;
            const size_t num_chars_to_move = m_prev_size_with_end_char - a_idx_dest - 1;
            std::memmove(dst, place_to_insert, num_chars_to_move * sizeof(char_type));
        }

        // Insert chars in empty space.
        const char_type* src = a_str + a_idx_src_start;
        std::memcpy(place_to_insert, src, num_chars_to_insert * sizeof(char_type));
        m_chars[m_chars.size() - 1] = char_end;

        chars_have_changed();
    }
    
    inline void string::replace_common(const char_type* a_str, size_t a_num_chars, size_t a_idx_dest_start,
        size_t a_idx_dest_end, size_t a_idx_src_start, size_t a_idx_src_end)
    {
        const size_t my_size = get_size();
        if(a_idx_dest_start == invalid_idx)
        {
            a_idx_dest_start = 0;
        }
        if(a_idx_dest_end == invalid_idx)
        {
            a_idx_dest_end = my_size - 1;
        }
        if(a_idx_src_start == invalid_idx)
        {
            a_idx_src_start = 0;
        }
        if(a_idx_src_end == invalid_idx)
        {
            a_idx_src_end = a_num_chars - 1;
        }
        JE_assert(a_idx_dest_start < my_size, "Invalid indices for string operation.");
        JE_assert(a_idx_dest_end < my_size, "Invalid indices for string operation.");
        JE_assert(a_idx_dest_start <= a_idx_dest_end, "Invalid indices for string operation.");
        JE_assert(a_idx_src_start < a_num_chars, "Invalid indices for string operation.");
        JE_assert(a_idx_src_end < a_num_chars, "Invalid indices for string operation.");
        JE_assert(a_idx_src_start <= a_idx_src_end, "Invalid indices for string operation.");

        const size_t num_chars_src = a_idx_src_end - a_idx_src_start + 1;
        const size_t num_chars_dst = a_idx_dest_end - a_idx_dest_start + 1;

        if(num_chars_dst >= num_chars_src)
        {
            // In this case we can simply copy over the existing characters without resizing.

            if(num_chars_dst > num_chars_src)
            {
                // We need to shrink the remaining characters.
            }
        }
        else
        {
            // We need to enlarge array AFTER dst and then copy.
        }
        /*
        const size_t total_chars_needed = a_idx_dest + num_chars_to_insert;
        if(total_chars_needed > my_size)
        {
            m_chars.resize(total_chars_needed + 1);
        }

        const char_type* src = a_str + a_idx_src_start;
        char_type* dst = m_chars.data() + a_idx_dest;
        std::memcpy(dst, src, num_chars_to_insert * sizeof(char_type));
        m_chars[m_chars.size() - 1] = char_end;
        */
    }

    inline bool string::find_and_replace_common(const char_type* a_str_to_find, size_t a_str_to_find_num_chars,
        const char_type* a_str_to_replace_it_with, size_t a_str_to_relace_it_with_num_chars)
    {
        size_t str_idx = 0;
        while((str_idx = find_common(a_str_to_find, a_str_to_find_num_chars, str_idx)) != invalid_idx)
        {
            //const size_t str_idx_prev = str_idx;
            //str_idx += a_str_to_find_num_chars;
            //replace_common()
        }

        if(str_idx != 0)
        {
            chars_have_changed();
            return true;
        }
        else
        {
            return false;
        }
    }

    inline size_t string::find_common(const char_type* a_str, size_t a_num_chars, size_t char_to_start_from/* = 0*/) const
    {
        JE_todo();
        return invalid_idx;
    }

    void string::chars_have_changed()
    {
        // TODO In future: Check what is faster : recomputing hash every time chars have changed
        // or maybe using dirty flag and delaying it until it's indeed needed for a comparison.
        build_hash();
    }

    inline void string::build_hash()
    {
        m_hash.build(get_data());
    }
    
}}