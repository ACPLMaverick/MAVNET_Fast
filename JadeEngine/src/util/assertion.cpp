#include "assertion.h"

#if JE_CONFIG_DEBUG

#include "platf/platf.h"
#include "platf/message_box.h"

#include <stdarg.h>

namespace je { namespace util {

    void assertion::create(const char* a_expression, const char* a_file, const char* a_function, size_t a_line, const char* a_message, ...)
    {
        data::string message = data::string::format(
            "Assertion failed!\n\nExpression: [%s]\nFile: [%s]\nFunction: [%s]\nLine: [%lld]",
            a_expression, a_file, a_function, a_line);
        if(a_message != nullptr && a_message[0] != '\0')
        {
            va_list args;
            va_start(args, a_message);
            data::string appended_message = data::string::format(
                a_message, args
            );
            va_end(args);
            message += "\n\n";
            message += appended_message;
        }

        static const char* separator = "######################################";
        JE_printf("\n%s\n%s\n%s\n\n", separator, message.get_data(), separator);

#if JE_ASSERTION_USES_MESSAGE_BOX
        {
            using namespace je::platf;
            const message_box::button_flag flags = 
            message_box::show(
                message.get_data(), 
                message_box::info_level::k_error,
                message_box::button_flag::k_continue | message_box::button_flag::k_retry | message_box::button_flag::k_cancel,
                true);
            if(flags & message_box::button_flag::k_retry)
            {
                je::platf::util::debugbreak();
            }
            else if(flags & message_box::button_flag::k_cancel)
            {
                std::exit(EXIT_FAILURE);
            }
        }
#else
    je::platf::util::debugbreak();
#endif
    }

    void assertion::create(const char* a_expression, const char* a_file, const char* a_function, size_t a_line)
    {
        create(a_expression, a_file, a_function, a_line, nullptr);
    }

}}

#endif