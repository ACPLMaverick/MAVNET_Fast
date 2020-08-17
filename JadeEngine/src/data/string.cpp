#include "string.h"

#include <cstring>
#include <stdarg.h>

namespace je { namespace data {

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

    int64_t string::parse_int64(const string& str)
    {
        JE_todo();
        return 0;
    }

    uint64_t string::parse_uint64(const string& str)
    {
        JE_todo();
        return 0;
    }

    int32_t string::parse_int32(const string& str)
    {
        JE_todo();
        return 0;
    }

    uint32_t string::parse_uint32(const string& str)
    {
        JE_todo();
        return 0;
    }

    float string::parse_float(const string& str)
    {
        JE_todo();
        return 0.0f;
    }

    void string::clear()
    {
        m_chars.clear();
        m_chars.push_back(char_end);
        chars_have_changed();
    }
    
    void string::resize(size_t a_num_chars, char_type a_char_to_fill/* = char_end*/)
    {
        JE_todo();
    }
    
    void string::reserve(size_t a_num_chars)
    {
        JE_todo();
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
        JE_todo();
        return false;
    }
    
    bool string::is_starting_with(const string& a_str) const
    {
        JE_todo();
        return false;
    }
    
    bool string::is_ending_with(const char_type* a_str) const
    {
        JE_todo();
        return false;
    }
    
    bool string::is_ending_with(const string& a_str) const
    {
        JE_todo();
        return false;
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
        JE_todo();
    }
    
    void string::append(const string& a_str)
    {
        JE_todo();
    }
    
    void string::operator+=(const char_type* a_str)
    {
        JE_todo();
    }
    
    void string::operator+=(const string& a_str)
    {
        JE_todo();
    }
    
    string string::operator+(const char_type* a_str)
    {
        JE_todo();
        return string();
    }
    
    string string::operator+(const string& a_str)
    {
        JE_todo();
        return string();
    }
    
    
    void string::insert(const char_type* a_str, size_t a_idx_dest/* = invalid_idx*/, size_t a_idx_src_start/* = invalid_idx*/, size_t a_idx_src_end/* = invalid_idx*/)
    {
        JE_todo();
    }
    
    void string::insert(const string& a_str, size_t a_idx_dest/* = invalid_idx*/, size_t a_idx_src_start/* = invalid_idx*/, size_t a_idx_src_end/* = invalid_idx*/)
    {
        JE_todo();
    }
    
    void string::replace(const char_type* a_str, size_t a_idx_dest/* = invalid_idx*/, size_t a_idx_src_start/* = invalid_idx*/, size_t a_idx_src_end/* = invalid_idx*/)
    {
        JE_todo();
    }
    
    void string::replace(const string& a_str, size_t a_idx_dest/* = invalid_idx*/, size_t a_idx_src_start/* = invalid_idx*/, size_t a_idx_src_end/* = invalid_idx*/)
    {
        JE_todo();
    }
    
    
    void string::swap(string& a_str)
    {
        JE_todo();
    }
    

    string string::substring(size_t a_idx_start, size_t a_idx_end) const
    {
        JE_todo();
        return string();
    }
    
    void string::make_substring(size_t a_idx_start, size_t a_idx_end)
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
        JE_todo();
        return invalid_idx;
    }
    
    size_t string::find(const string& a_str) const
    {
        JE_todo();
        return invalid_idx;
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
    

    void string::create_from_str(const char_type* a_str)
    {
        // TODO Check what is faster: strlen and memcpy or adding chars one by one.
        if(a_str == nullptr)
        {
            m_chars.push_back(char_end);
            return;
        }

        do
        {
            m_chars.push_back(*a_str);
        } while(*(++a_str) != char_end);
        m_chars.push_back(char_end);
        chars_have_changed();
    }

    void string::chars_have_changed()
    {
        // TODO In future: Check what is faster : recomputing hash every time chars have changed
        // or maybe using dirty flag and delaying it until it's indeed needed for a comparison.
        build_hash();
    }

    void string::build_hash()
    {
        m_hash.build(get_data());
    }
    
}}