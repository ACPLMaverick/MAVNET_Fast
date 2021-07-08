#pragma once

#include "global.h"

namespace je { namespace util {

    class misc
    {
    public:
        static data::string get_file_separator();
        static data::string call_system_command(const data::string& command);
        static void debugbreak();
        static const char* trim_file_name(const char* absolute_file_name);

        template<typename integral_type>
        static u8 get_num_bits(integral_type a_value)
        {
            u8 count = 0;
            while(a_value != 0)
            {
                a_value &= (a_value - 1);
                ++count;
            }
            return count;
        }
    };

#define JE_array_num(_array_) (sizeof(_array_) / sizeof(_array_[0]))

}}