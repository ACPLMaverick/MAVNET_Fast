#pragma once

#include "global.h"

namespace je { namespace data {

    class string_hash
    {
    private:

        // uint32_t m_value;
    };

    class string
    {
    public:
        using char_type = char;

        string();
        string(const char* str);
        ~string();

    private:

        array<char_type> m_chars;
        // string_hash m_hash;
    };

}}