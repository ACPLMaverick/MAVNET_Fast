#pragma once

#include "global.h"

namespace je { namespace data {

    class string
    {
    public:
        using char_type = char;
        static const size_t invalid_idx = std::numeric_limits<size_t>::max();
        static const char char_end = '\0';

    private:

        class hash
        {
        public:

            hash(const char_type* str);
            hash(const hash& other);
            hash(hash&& move);
            hash& operator=(const hash& other);
            hash& operator=(hash&& move);

            void build(const char_type* str);

            operator uint32_t() const { return m_value; }

        private:

            uint32_t m_value;
        };

    public:

        string();
        string(const char_type* str);
        string(const string& copy);
        string(string&& move);
        ~string();
        string& operator=(const char_type* str);
        string& operator=(const string& copy);
        string& operator=(const string&& move);

        static string format(const char_type* format, const char_type* args...);
        static string from_int64(int64_t value);
        static string from_uint64(uint64_t value);
        static string from_int32(int64_t value);
        static string from_uint32(uint64_t value);
        static string from_float(float value, size_t decimal_places = std::numeric_limits<size_t>::max());

        void clear();
        void resize(size_t num_chars, char_type char_to_fill = char_end);
        void reserve(size_t num_chars);

        size_t get_size() const;
        bool is_empty() const;
        bool is_starting_with(const char_type* str) const;
        bool is_starting_with(const string& str) const;
        bool is_ending_with(const char_type* str) const;
        bool is_ending_with(const string& str) const;

        bool operator==(const char_type* str) const;
        bool operator==(const string& str) const;

        char_type operator[](size_t index) const;
        const char_type* get_data() const;
        const char_type* operator*() const;

        void append(const char_type* str);
        void append(const string& str);
        void operator+=(const char_type* str);
        void operator+=(const string& str);
        string operator+(const char_type* str);
        string operator+(const string& str);
        
        void insert(const char_type* str, size_t idx_dest = invalid_idx, size_t idx_src_start = invalid_idx, size_t idx_src_end = invalid_idx);
        void insert(const string& str, size_t idx_dest = invalid_idx, size_t idx_src_start = invalid_idx, size_t idx_src_end = invalid_idx);
        void replace(const char_type* str, size_t idx_dest = invalid_idx, size_t idx_src_start = invalid_idx, size_t idx_src_end = invalid_idx);
        void replace(const string& str, size_t idx_dest = invalid_idx, size_t idx_src_start = invalid_idx, size_t idx_src_end = invalid_idx);
        
        void swap(string& str);

        string substring(size_t idx_start, size_t idx_end) const;
        void make_substring(size_t idx_start, size_t idx_end);
        void trim_front(size_t num_chars);
        void trim_end(size_t num_chars);
        void split(const char_type* split_on, array<string> out_strings) const;
        void split(const string& split_on, array<string> out_strings) const;

        size_t find(const char_type* str) const;
        size_t find(const string& str) const;
        bool contains(const char_type* str) const;
        bool contains(const string& str) const;

        void to_upper_case();
        void to_lower_case();
        void to_capitalized_case();
        bool is_upper_case() const;
        bool is_lower_case() const;

    private:

        void create_from_str(const char_type* str);
        void chars_have_changed();
        void build_hash();

        array<char_type> m_chars;
        hash m_hash;
    };

}}