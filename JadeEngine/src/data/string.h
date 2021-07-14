#pragma once

#include "global.h"

namespace je { namespace data {

    class string
    {
    public:
        using char_type = char;
        static const size invalid_idx = std::numeric_limits<size>::max();
        static const char char_end = '\0';
        static const char char_space = ' ';

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

            operator u32() const { return m_value; }

        private:

            u32 m_value;
        };

    public:

        string();
        string(const char_type* str);
    private:
        string(size num_chars_to_have);
    public:
        string(const string& copy);
        string(string&& move);
        ~string();
        string& operator=(const char_type* str);
        string& operator=(const string& copy);
        string& operator=(const string&& move);

        static string format(const char_type* format, ...);
        static string format(const char_type* format, va_list args);
        static string from_int64(i64 value);
        static string from_uint64(u64 value);
        static string from_int32(i32 value);
        static string from_uint32(u32 value);
        static string from_float(f32 value, size decimal_places = std::numeric_limits<size>::max());
        static string from_substring(const char_type* str, size idx_start, size idx_end);
        static string from_substring(const string& str, size idx_start, size idx_end);

        static i64 parse_int64(const char_type* str);
        static u64 parse_uint64(const char_type* str);
        static i32 parse_int32(const char_type* str);
        static u32 parse_uint32(const char_type* str);
        static f32 parse_float(const char_type* str);
        static i64 parse_int64(const string& str);
        static u64 parse_uint64(const string& str);
        static i32 parse_int32(const string& str);
        static u32 parse_uint32(const string& str);
        static f32 parse_float(const string& str);

        static bool compare(const char_type* a, const char_type* b);
        static bool compare_case_insensitive(const char_type* a, const char_type* b);
        static size compute_size(const char_type* str);

        void clear();
        void resize(size num_chars, char_type char_to_fill = char_end);
        void reserve(size num_chars);

        size get_size() const;
        bool is_empty() const;
        bool is_starting_with(const char_type* str) const;
        bool is_starting_with(const string& str) const;
        bool is_ending_with(const char_type* str) const;
        bool is_ending_with(const string& str) const;

        bool operator==(const char_type* str) const;
        bool operator==(const string& str) const;

        char_type operator[](size index) const;
        const char_type* get_data() const;
        const char_type* operator*() const;

        void append(const char_type* str);
        void append(const string& str);
        void operator+=(const char_type* str);
        void operator+=(const string& str);
        string operator+(const char_type* str);
        string operator+(const string& str);
        
        // All indices are inclusive.
        void insert(const char_type* str, size idx_dest = invalid_idx, size idx_src_start = invalid_idx, size idx_src_end = invalid_idx);
        void insert(const string& str, size idx_dest = invalid_idx, size idx_src_start = invalid_idx, size idx_src_end = invalid_idx);
        void replace(const char_type* str, size idx_dest_start = invalid_idx, size idx_dest_end = invalid_idx, size idx_src_start = invalid_idx, size idx_src_end = invalid_idx);
        void replace(const string& str, size idx_dest_start = invalid_idx, size idx_dest_end = invalid_idx, size idx_src_start = invalid_idx, size idx_src_end = invalid_idx);

        void erase(size idx_start, size idx_end = invalid_idx);
        
        bool find_and_replace(const char_type* str_to_find, const char_type* str_to_replace_it_with);
        bool find_and_replace(const string& str_to_find, const string& str_to_replace_it_with);
        bool find_and_replace(const string& str_to_find, const char_type* str_to_replace_it_with);
        bool find_and_replace(const char_type* str_to_find, const string& str_to_replace_it_with);

        void swap(string& str);

        void substring(size idx_start, size idx_end);
        void trim_front(size num_chars);
        void trim_end(size num_chars);
        void split(const char_type* split_on, array<string>& out_strings) const;
        void split(const string& split_on, array<string>& out_strings) const;

        size find(const char_type* str) const;
        size find(const string& str) const;
        bool contains(const char_type* str) const;
        bool contains(const string& str) const;

        void to_upper_case();
        void to_lower_case();
        void to_capitalized_case();
        bool is_upper_case() const;
        bool is_lower_case() const;

    private:

        static inline string from_substring_common(const char_type* str, size num_chars, size idx_start, size idx_end);
        inline void append_common(const char_type* str, size num_chars);
        inline void insert_common(const char_type* str, size num_chars, size idx_dest, size idx_src_start, size idx_src_end);
        inline void replace_common(const char_type* str, size num_chars, size idx_dest_start, size idx_dest_end, size idx_src_start, size idx_src_end);
        inline void erase_common(size idx_start, size idx_end = invalid_idx);
        inline bool find_and_replace_common(const char_type* str_to_find, size str_to_find_num_chars, const char_type* str_to_replace_it_with, size str_to_replace_it_with_num_chars);
        inline size find_common(const char_type* str, size num_chars, size char_to_start_from = 0) const;
        inline void split_common(const char_type* split_on, size split_on_num_chars, array<string>& out_strings) const;
        
        inline void create_from_str(const char_type* str, size idx_start, size idx_end = invalid_idx);
        void chars_have_changed();
        inline void build_hash();
        inline void remove_excessive_chars_at_end(size new_char_num);
        inline void shrink_if_necessary();

        template<typename in_char_type> static bool is_lower_case(in_char_type value);
        template<typename in_char_type> static bool is_upper_case(in_char_type value);
        template<typename in_char_type> static in_char_type to_upper_case(in_char_type value);
        template<typename in_char_type> static in_char_type to_lower_case(in_char_type value);

        array<char_type> m_chars;
        hash m_hash;
    };
}}