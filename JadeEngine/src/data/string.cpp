#include "string.h"

#include <cstring>
#include <cstdarg>
#include <cstdio>

namespace je { namespace data {

#define JE_check_str_bailout_ret(_str_, _ret_val_) if(_str_ == nullptr || _str_[0] == char_end) return _ret_val_
#define JE_check_str_bailout(_str_) if(_str_ == nullptr || _str_[0] == char_end) return

    // Case support for "char" type.
    template<> bool string::is_lower_case(char value)
    {
        return value > 0x60 && value < 0x7B;
    }

    template<> bool string::is_upper_case(char value)
    {
        return value > 0x40 && value < 0x5B;
    }

    template<> char string::to_upper_case(char value)
    {
        return value - 0x20;
    }

    template<> char string::to_lower_case(char value)
    {
        return value + 0x20;
    }
    // /////////////

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
            u32 c = 0;
            while ((c = *(a_str++)) != 0)
                m_value = ((m_value << 5) + m_value) + c; /* hash * 33 + c */
        }
        else
        {
            m_value = 0;
        }
    }

    string::string()
        : m_chars(1)
        , m_hash(get_data())
    {
        JE_assert(m_chars[0] == char_end);
    }

    string::string(const char_type* a_str)
        : m_hash(nullptr)
    {
        create_from_str(a_str, 0);
    }

    string::string(size a_num_chars_to_have)
        : m_chars()
        , m_hash(nullptr)
    {
        m_chars.resize(a_num_chars_to_have + 1);
        m_chars[a_num_chars_to_have] = char_end;
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
        create_from_str(a_str, 0);
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
        string new_string = format(a_format, args);
        va_end(args);
        return new_string;
    }

    string string::format(const char_type* a_format, va_list a_args)
    {
#if JE_PLATFORM_LINUX // va_list seems to be one-use-only on Linux.
        va_list args_copy;
        va_copy(args_copy, a_args);
#endif
        const int char_num = vsnprintf(nullptr, 0, a_format, a_args);

        JE_assert(char_num >= 0, "Failed to format a string.");
        string str = char_num > 0 ? string(char_num) : string(nullptr);
        if(char_num > 0)
        {
            vsnprintf(str.m_chars.data(), char_num + 1, a_format, 
#if JE_PLATFORM_LINUX
                args_copy
#else
                a_args
#endif
                );
            str.chars_have_changed();
        }

#if JE_PLATFORM_LINUX
        va_end(args_copy);
#endif
        return str;
    }
    
    string string::from_int64(i64 a_value)
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
            string str(char_num);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_uint64(u64 a_value)
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
            string str(char_num);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_int32(i32 a_value)
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
            string str(char_num);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_uint32(u32 a_value)
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
            string str(char_num);
            std::snprintf(str.m_chars.data(), char_num + 1, k_fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }
    
    string string::from_float(f32 a_value, size a_decimal_places/* = std::numeric_limits<size>::max()*/)
    {
        static const char_type* k_fmt_base = "%f";
        static const size k_fmt_dec_buf_size = 6;
        static thread_local char_type k_fmt_dec_buf[k_fmt_dec_buf_size] = {};

        const char_type* fmt = k_fmt_base;
        if(a_decimal_places != std::numeric_limits<size>::max())
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
            string str(char_num);
            std::snprintf(str.m_chars.data(), char_num + 1, fmt, a_value);
            str.chars_have_changed();
            return str;
        }
    }

    string string::from_substring(const char_type* a_str, size a_idx_start, size a_idx_end)
    {
        return from_substring_common(a_str, std::strlen(a_str), a_idx_start, a_idx_end);
    }

    string string::from_substring(const string& a_str, size a_idx_start, size a_idx_end)
    {
        return from_substring_common(a_str.get_data(), a_str.get_size(), a_idx_start, a_idx_end);
    }

    i64 string::parse_int64(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        i64 val = std::strtoll(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    u64 string::parse_uint64(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        u64 val = std::strtoull(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    i32 string::parse_int32(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        i32 val = std::strtol(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    u32 string::parse_uint32(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        u32 val = std::strtoul(a_str, nullptr, 10);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    f32 string::parse_float(const char_type* a_str)
    {
        JE_assert(a_str != nullptr, "Passing a null c-string to parse function.");
        f32 val = std::strtof(a_str, nullptr);
        JE_assert(errno != ERANGE, "Parsing a number [%s] failed.", a_str);
        return val;
    }

    i64 string::parse_int64(const string& a_str)
    {
        return parse_int64(a_str.get_data());
    }

    u64 string::parse_uint64(const string& a_str)
    {
        return parse_uint64(a_str.get_data());
    }

    i32 string::parse_int32(const string& a_str)
    {
        return parse_int32(a_str.get_data());
    }

    u32 string::parse_uint32(const string& a_str)
    {
        return parse_uint32(a_str.get_data());
    }

    f32 string::parse_float(const string& a_str)
    {
        return parse_float(a_str.get_data());
    }

    bool string::compare(const char_type* a, const char_type* b)
    {
        return std::strcmp(a, b) == 0;
    }

    bool string::compare_case_insensitive(const char_type* a, const char_type* b)
    {
        JE_todo();
        return false;
    }

    size string::compute_size(const char_type* str)
    {
        return std::strlen(str);
    }

    void string::clear()
    {
        m_chars.clear();
        const char val = string::char_end;
        m_chars.push_back(val);
        chars_have_changed();
    }
    
    void string::resize(size a_num_chars, char_type a_char_to_fill/* = char_end*/)
    {
        m_chars.resize(a_num_chars + 1);
        std::memset(m_chars.data(), static_cast<int>(a_char_to_fill), a_num_chars * sizeof(char_type));
        m_chars[m_chars.size() - 1] = char_end;
        chars_have_changed();
    }
    
    void string::reserve(size a_num_chars)
    {
        m_chars.reserve(a_num_chars + 1);
    }
    

    size string::get_size() const
    {
        // TODO: Better idea to use strlen for this?
        JE_assert(m_chars.size() > 0, "Bad chars array size.");
        return m_chars.size() - 1;
    }

    string::hash string::get_hash() const
    {
        return m_hash;
    }
    
    bool string::is_empty() const
    {
        return get_size() == 0;
    }
    
    bool string::is_starting_with(const char_type* a_str) const
    {
        JE_check_str_bailout_ret(a_str, false);

        const size my_size = get_size();
        size i = 0;
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

        const size other_size = std::strlen(a_str);
        const size my_size = get_size();

        size io = other_size - 1;
        size im = my_size - 1;
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
        return im < my_size || (im == std::numeric_limits<size>::max() && io == std::numeric_limits<size>::max());
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
    

    string::char_type string::operator[](size a_index) const
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
        const size num_chars = std::strlen(a_str);
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
    
    
    void string::insert(const char_type* a_str, size a_idx_dest/* = invalid_idx*/,
        size a_idx_src_start/* = invalid_idx*/, size a_idx_src_end/* = invalid_idx*/)
    {
        JE_check_str_bailout(a_str);
        const size num_chars = std::strlen(a_str);
        insert_common(a_str, num_chars, a_idx_dest, a_idx_src_start, a_idx_src_end);
    }
    
    void string::insert(const string& a_str, size a_idx_dest/* = invalid_idx*/,
        size a_idx_src_start/* = invalid_idx*/, size a_idx_src_end/* = invalid_idx*/)
    {
        insert_common(a_str.get_data(), a_str.get_size(), a_idx_dest, a_idx_src_start, a_idx_src_end);
    }
    
    void string::replace(const char_type* a_str, size a_idx_dest_start/* = invalid_idx*/,
        size a_idx_dest_end/* = invalid_idx*/, size a_idx_src_start/* = invalid_idx*/,
        size a_idx_src_end/* = invalid_idx*/)
    {
        JE_check_str_bailout(a_str);
        const size num_chars = std::strlen(a_str);
        replace_common(a_str, num_chars, a_idx_dest_start, a_idx_dest_end, a_idx_src_start, a_idx_src_end);
        chars_have_changed();
    }
    
    void string::replace(const string& a_str, size a_idx_dest_start/* = invalid_idx*/,
        size a_idx_dest_end/* = invalid_idx*/,
        size a_idx_src_start/* = invalid_idx*/, size a_idx_src_end/* = invalid_idx*/)
    {
        replace_common(a_str.get_data(), a_str.get_size(), a_idx_dest_start, a_idx_dest_end, a_idx_src_start, a_idx_src_end);
        chars_have_changed();
    }


    void string::erase(size a_idx_start, size a_idx_end)
    {
        erase_common(a_idx_start, a_idx_end);
        chars_have_changed();
    }


    bool string::find_and_replace(const char_type* a_str_to_find, const char_type* a_str_to_replace_it_with)
    {
        JE_check_str_bailout_ret(a_str_to_find, false);
        // To_replace_it_with can be an empty string.
        JE_assert_bailout(a_str_to_replace_it_with != nullptr, false, "Null argument.");
        const size num_chars_to_find = std::strlen(a_str_to_find);
        const size num_chars_to_replace = std::strlen(a_str_to_replace_it_with);
        return find_and_replace_common(a_str_to_find, num_chars_to_find, a_str_to_replace_it_with, num_chars_to_replace);
    }

    bool string::find_and_replace(const string& a_str_to_find, const string& a_str_to_replace_it_with)
    {
        return find_and_replace_common(a_str_to_find.get_data(), a_str_to_find.get_size(), a_str_to_replace_it_with.get_data(), a_str_to_replace_it_with.get_size());
    }

    bool string::find_and_replace(const string& a_str_to_find, const char_type* a_str_to_replace_it_with)
    {
        // To_replace_it_with can be an empty string.
        JE_assert_bailout(a_str_to_replace_it_with != nullptr, false, "Null argument.");
        const size num_chars_to_replace = std::strlen(a_str_to_replace_it_with);
        return find_and_replace_common(a_str_to_find.get_data(), a_str_to_find.get_size(), a_str_to_replace_it_with, num_chars_to_replace);
    }

    bool string::find_and_replace(const char_type* a_str_to_find, const string& a_str_to_replace_it_with)
    {
        JE_check_str_bailout_ret(a_str_to_find, false);
        const size num_chars_to_find = std::strlen(a_str_to_find);
        return find_and_replace_common(a_str_to_find, num_chars_to_find, a_str_to_replace_it_with.get_data(), a_str_to_replace_it_with.get_size());
    }
    
    
    void string::swap(string& a_str)
    {
        if(*this == a_str)
        {
            return;
        }

        const size my_char_num = m_chars.size();
        const size other_char_num = a_str.m_chars.size();

        if(other_char_num > my_char_num)
        {
            m_chars.resize(other_char_num);
        }
        else if(my_char_num > other_char_num)
        {
            a_str.m_chars.resize(my_char_num);
        }

        {
            string copy(*this);
            std::memcpy(&m_chars[0], &a_str.m_chars[0], other_char_num * sizeof(char_type));
            std::memcpy(&a_str.m_chars[0], &copy.m_chars[0], my_char_num * sizeof(char_type));
        }

        remove_excessive_chars_at_end(other_char_num);
        a_str.remove_excessive_chars_at_end(my_char_num);
        chars_have_changed();
        a_str.chars_have_changed();
    }

    
    void string::substring(size a_idx_start, size a_idx_end)
    {
        JE_assert_bailout(a_idx_start < get_size(), , "Invalid argument.");
        JE_assert_bailout(a_idx_end < get_size(), , "Invalid argument.");
        JE_assert_bailout(a_idx_end >= a_idx_start, , "Invalid argument.");

        const size new_size = a_idx_end - a_idx_start + 1;

        // If necessary, move the substring block to the front, and then erase remaining chars.
        if(a_idx_start > 0)
        {
            char_type* move_dest = &m_chars[0];
            const char_type* move_src = &m_chars[a_idx_start];
            std::memmove(move_dest, move_src, new_size * sizeof(char_type));
        }

        // Add a new trailing zero at the end.
        m_chars[new_size] = char_end;
        remove_excessive_chars_at_end(new_size + 1);
        chars_have_changed();
    }
    
    void string::trim_front(size a_num_chars)
    {
        substring(a_num_chars, get_size() - 1);
    }
    
    void string::trim_end(size a_num_chars)
    {
        substring(0, get_size() - (a_num_chars + 1));
    }
    
    void string::split(const char_type* a_split_on, array<string>& a_out_strings) const
    {
        split_common(a_split_on, std::strlen(a_split_on), a_out_strings);
    }
    
    void string::split(const string& a_split_on, array<string>& a_out_strings) const
    {
        split_common(a_split_on.get_data(), a_split_on.get_size(), a_out_strings);
    }
    

    size string::find(const char_type* a_str) const
    {
        JE_check_str_bailout_ret(a_str, invalid_idx);
        const size num_chars = std::strlen(a_str);
        return find_common(a_str, num_chars);
    }
    
    size string::find(const string& a_str) const
    {
        return find_common(a_str.get_data(), a_str.get_size());
    }
    
    bool string::contains(const char_type* a_str) const
    {
        return find(a_str) != invalid_idx;
    }
    
    bool string::contains(const string& a_str) const
    {
        return find(a_str) != invalid_idx;
    }
    

    void string::to_upper_case()
    {
        bool chars_changed = false;
        const size my_size = get_size();

        for(size i = 0; i < my_size; ++i)
        {
            if(is_lower_case(m_chars[i]))
            {
                m_chars[i] = to_upper_case(m_chars[i]);
                chars_changed = true;
            }
        }

        if(chars_changed)
        {
            chars_have_changed();
        }
    }
    
    void string::to_lower_case()
    {
        bool chars_changed = false;
        const size my_size = get_size();

        for(size i = 0; i < my_size; ++i)
        {
            if(is_upper_case(m_chars[i]))
            {
                m_chars[i] = to_lower_case(m_chars[i]);
                chars_changed = true;
            }
        }

        if(chars_changed)
        {
            chars_have_changed();
        }
    }
    
    void string::to_capitalized_case()
    {
        bool chars_changed = false;
        const size my_size = get_size();
        bool should_capitalize = true;

        for(size i = 0; i < my_size; ++i)
        {
            const bool is_upper = is_upper_case(m_chars[i]);
            const bool is_lower = is_lower_case(m_chars[i]);
            const bool is_character = is_upper || is_lower;

            if(is_character)
            {
                if(should_capitalize)
                {
                    if(is_lower)
                    {
                        m_chars[i] = to_upper_case(m_chars[i]);
                        should_capitalize = false;
                        chars_changed = true;
                    }
                    else if(is_upper)
                    {
                        should_capitalize = false;
                    }
                }
                else
                {
                    if(is_upper)
                    {
                        m_chars[i] = to_lower_case(m_chars[i]);
                        chars_changed = true;
                    }
                }
            }
            else
            {
                should_capitalize = true;
            }
        }

        if(chars_changed)
        {
            chars_have_changed();
        }
    }
    
    bool string::is_upper_case() const
    {
        const size my_size = get_size();
        for(size i = 0; i < my_size; ++i)
        {
            if(is_lower_case(m_chars[i]))
            {
                return false;
            }
        }
        return true;
    }
    
    bool string::is_lower_case() const
    {
        const size my_size = get_size();
        for(size i = 0; i < my_size; ++i)
        {
            if(is_upper_case(m_chars[i]))
            {
                return false;
            }
        }
        return true;
    }


    inline string string::from_substring_common(const char_type* a_str, size a_num_chars, size a_idx_start, size a_idx_end)
    {
        JE_assert_bailout(a_idx_start < a_num_chars, string(nullptr), "Invalid argument.");
        JE_assert_bailout(a_idx_end < a_num_chars, string(nullptr), "Invalid argument.");
        JE_assert_bailout(a_idx_end >= a_idx_start, string(nullptr), "Invalid argument.");

        const size new_size = a_idx_end - a_idx_start + 1;
        string new_string(new_size);
        std::memcpy(&new_string.m_chars[0], a_str + a_idx_start, new_size * sizeof(char_type));
        new_string.m_chars[new_string.m_chars.size() - 1] = char_end;
        new_string.chars_have_changed();

        return new_string;
    }

    inline void string::append_common(const char_type* a_str, size a_num_chars)
    {
        const size prev_zero_idx = get_size();
        m_chars.resize(m_chars.size() + a_num_chars);
        std::memcpy(&m_chars[prev_zero_idx], a_str, a_num_chars * sizeof(char_type));
        m_chars[m_chars.size() - 1] = char_end;

        chars_have_changed();
    }

    inline void string::insert_common(const char_type* a_str, size a_num_chars,
        size a_idx_dest, size a_idx_src_start, size a_idx_src_end)
    {
        const size my_size = get_size();
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

        const size num_chars_to_insert = a_idx_src_end - a_idx_src_start + 1;

        const size m_prev_size_with_end_char = m_chars.size();
        m_chars.resize(m_chars.size() + num_chars_to_insert);
        char_type* place_to_insert = &m_chars[a_idx_dest];
        if(a_idx_dest != my_size)
        {
            // Want to insert chars in the middle of the string - need to move the remaining part first.
            char_type* dst = place_to_insert + num_chars_to_insert;
            const size num_chars_to_move = m_prev_size_with_end_char - a_idx_dest - 1;
            std::memmove(dst, place_to_insert, num_chars_to_move * sizeof(char_type));
        }

        // Insert chars in empty space.
        const char_type* src = a_str + a_idx_src_start;
        std::memcpy(place_to_insert, src, num_chars_to_insert * sizeof(char_type));
        m_chars[m_chars.size() - 1] = char_end;

        chars_have_changed();
    }
    
    inline void string::replace_common(const char_type* a_str, size a_num_chars, size a_idx_dest_start,
        size a_idx_dest_end, size a_idx_src_start, size a_idx_src_end)
    {
        const size my_size = get_size();
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

        const size num_chars_src = a_idx_src_end - a_idx_src_start + 1;
        const size num_chars_dst = a_idx_dest_end - a_idx_dest_start + 1;

        if(num_chars_dst > num_chars_src)
        {
            // We need to shrink the remaining characters.
            const size diff = num_chars_dst - num_chars_src;
            char_type* move_source = &m_chars[0] + a_idx_dest_end;
            char_type* move_dest = move_source - diff;
            std::memmove(move_dest, move_source, my_size - a_idx_dest_end + 1);

            // When resizing downwards there should be no deallocation.
            m_chars.resize(m_chars.size() - diff);
            // Shrink to fit only in really heavy cases as it may cause a reallocation.
            shrink_if_necessary();
        }
        else
        {
            // We need to enlarge array AFTER dst and then copy.
            const size diff = num_chars_src - num_chars_dst;
            m_chars.resize(m_chars.size() + diff);  // Possible reallocation.
            char_type* move_source = &m_chars[0] + a_idx_dest_end + 1;
            char_type* move_dest = move_source + diff;
            std::memmove(move_dest, move_source, my_size - a_idx_dest_end + 1);
        }

        // After necessary moves, we can simply copy over the source.
        char_type* dest = &m_chars[0] + a_idx_dest_start;
        char_type* src = const_cast<char_type*>(a_str) + a_idx_src_start;
        std::memcpy(dest, src, num_chars_src);
    }

    inline void string::erase_common(size a_idx_start, size a_idx_end)
    {
        const size my_size = get_size();
        JE_assert_bailout(a_idx_start < my_size, , "Invalid argument.");
        JE_assert_bailout(a_idx_end >= a_idx_start, , "Invalid argument.");

        if(a_idx_end >= my_size)
        {
            a_idx_end = my_size - 1;
        }

        // We need basically to move the remainder and shrink the string by this number of chars.

        const size move_amount = a_idx_end - a_idx_start + 1;
        char_type* move_dest = &m_chars[a_idx_start];
        const char_type* move_src = move_dest + move_amount;
        const size chars_to_move = &(m_chars[m_chars.size() - 1]) - move_src + 1;
        std::memmove(move_dest, move_src, chars_to_move * sizeof(char_type));

        remove_excessive_chars_at_end(my_size - move_amount + 1);
    }

    inline bool string::find_and_replace_common(const char_type* a_str_to_find, size a_str_to_find_num_chars,
        const char_type* a_str_to_replace_it_with, size a_str_to_replace_it_with_num_chars)
    {
        size str_idx = 0;
        while((str_idx = find_common(a_str_to_find, a_str_to_find_num_chars, str_idx)) != invalid_idx)
        {
            if(a_str_to_replace_it_with_num_chars > 0)
            {
                replace_common(a_str_to_replace_it_with, a_str_to_replace_it_with_num_chars,
                    str_idx, str_idx + a_str_to_find_num_chars - 1, 0, a_str_to_replace_it_with_num_chars - 1);
                str_idx += a_str_to_replace_it_with_num_chars;
            }
            else
            {
                // If replacing with an empty string, simply erase chars at this index.
                erase_common(str_idx, str_idx + a_str_to_find_num_chars - 1);
            }
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

    inline size string::find_common(const char_type* a_str, size a_num_chars, size a_char_to_start_from/* = 0*/) const
    {
        // Simple forward search for now.
        size current_char_idx = a_char_to_start_from;
        while(current_char_idx < (m_chars.size() - 1))
        {
            const char_type* ptr = &m_chars[current_char_idx];
            JE_unused(ptr);
            size num_traversed = 0;
            bool is_find_failed = false;
            while(num_traversed < a_num_chars)
            {
                is_find_failed = m_chars[current_char_idx + num_traversed] != a_str[num_traversed];
                ++num_traversed;
                if(is_find_failed)
                {
                    break;
                }
            }

            // String has been found here.
            if(is_find_failed == false)
            {
                return current_char_idx;
            }
            else
            {
                current_char_idx += num_traversed;
            }
        }
        return invalid_idx;
    }

    inline void string::split_common(const char_type* a_split_on, size a_split_on_num_chars, array<string>& a_out_strings) const
    {
        size prev_found_idx = invalid_idx;
        size curr_found_idx = 0;
        while((curr_found_idx = find_common(a_split_on, a_split_on_num_chars, curr_found_idx)) != invalid_idx)
        {
            // End-idx is exclusive here.
            const size start_idx = (prev_found_idx != invalid_idx ? (prev_found_idx + a_split_on_num_chars) : 0);
            const size end_idx = curr_found_idx;
            const size part_num_chars = end_idx - start_idx;
            if(part_num_chars > 0)
            {
                string new_str(from_substring_common(get_data(), get_size(), start_idx, end_idx - 1));
                a_out_strings.push_back(new_str);
            }
            prev_found_idx = curr_found_idx;
            curr_found_idx += a_split_on_num_chars;
        }

        if(prev_found_idx != 0)
        {
            // If this is different than zero, it means we need to add the last part - from this idx to the end.
            const size end_idx = get_size();
            if(end_idx > (prev_found_idx + a_split_on_num_chars))
            {
                const size last_part_num_chars = end_idx - (prev_found_idx + a_split_on_num_chars);
                if(last_part_num_chars > 0)
                {
                    string new_str(from_substring_common(get_data(), get_size(), prev_found_idx + a_split_on_num_chars, end_idx - 1));
                    a_out_strings.push_back(new_str);
                }
            }
        }
    }

    inline void string::create_from_str(const char_type* a_str, size a_idx_start, size a_idx_end /*= invalid_idx*/)
    {
        JE_assert(a_idx_end >= a_idx_start, "Invalid argument.");
        if(a_str == nullptr
            || a_idx_end < a_idx_start)
        {
            const char val = string::char_end;
            m_chars.push_back(val);
            return;
        }

        // TODO Check what is faster: strlen and memcpy or adding chars one by one.
        /*
        do
        {
            m_chars.push_back(*a_str);
        } while(*(++a_str) != char_end);
        */
        const size len = (a_idx_end != invalid_idx ? a_idx_end : std::strlen(a_str)) - a_idx_start;
        JE_assert(len > 0, "Invalid input string.");
        if(len > 0)
        {
            m_chars.resize(len + 1);
            std::memcpy(m_chars.data(), a_str + a_idx_start, len * sizeof(char_type));
        }
        m_chars[m_chars.size() - 1] = char_end;

        chars_have_changed();
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

    inline void string::remove_excessive_chars_at_end(size a_new_char_num)
    {
        if(m_chars.size() > a_new_char_num)
        {
            m_chars.erase(m_chars.begin() + a_new_char_num, m_chars.end());
            shrink_if_necessary();
        }
    }

    inline void string::shrink_if_necessary()
    {
        if(m_chars.capacity() > (m_chars.size() / 2))
        {
            m_chars.shrink_to_fit();
        }
    }
    
}}