#include "global.h"
#include "assertion.h"

#if JE_CONFIG_DEBUG

#include "global.h"
#include "util/message_box.h"
#include "util/misc.h"

#include <cstdarg>

namespace je { namespace util {

    void assertion::create(const char* a_expression, const char* a_file, const char* a_function, size a_line, const char* a_message, ...)
    {
        // I don't use data::string here to avoid circular dependency when some assertion
        // fires during engine initialization.

        static data::static_array<char, 1024> format_buf;
        size num_chars = snprintf(format_buf.get_data(), format_buf.k_num_objects, 
            "Assertion failed!\n\nExpression: [%s]\nFile: [%s]\nFunction: [%s]\nLine: [%zd]",
            a_expression, a_file, a_function, a_line);

        static const char* newline = "\n\n";
        if(a_message != nullptr && a_message[0] != '\0' && (num_chars + strlen(newline)) < format_buf.k_num_objects)
        {
            num_chars += snprintf(&format_buf[num_chars], format_buf.k_num_objects - num_chars, "\n\n");
            
            va_list args;
            va_start(args, a_message);
            vsnprintf(&format_buf[num_chars], format_buf.k_num_objects - num_chars, a_message, args);
            va_end(args);
        }

        static const char* separator = "######################################";
        JE_print("\n%s\n%s\n%s\n\n", separator, format_buf.get_data(), separator);
        JE_print_flush();

#if JE_ASSERTION_USES_MESSAGE_BOX
        {
            using namespace je::util;
            const message_box::button_flag flags = 
            message_box::show(
                a_message, 
                message_box::info_level::k_error,
                message_box::button_flag::k_continue | message_box::button_flag::k_retry | message_box::button_flag::k_cancel,
                true);
            if(flags & message_box::button_flag::k_retry)
            {
                je::util::misc::debugbreak();
            }
            else if(flags & message_box::button_flag::k_cancel)
            {
                std::exit(EXIT_FAILURE);
            }
        }
#else
    je::util::misc::debugbreak();
#endif
    }

    void assertion::create(const char* a_expression, const char* a_file, const char* a_function, size a_line)
    {
        create(a_expression, a_file, a_function, a_line, nullptr);
    }

}}

#endif