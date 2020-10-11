#pragma once

#include <cstdlib>

#if JE_CONFIG_DEBUG

// Not supported for now on Linux, at least until I get a window implementation.
#define JE_ASSERTION_USES_MESSAGE_BOX (1 && (JE_PLATFORM_WINDOWS))

namespace je { namespace util {

    class assertion
    {
    public:
        static void create(const char* expression, const char* file, const char* function, size_t line, const char* message, ...);
        static void create(const char* expression, const char* file, const char* function, size_t line);
    };

}}

#endif

#if JE_CONFIG_DEBUG
#define JE_assert(_expr_, ...) { auto __ret__ = (_expr_); if(!(__ret__)) { je::util::assertion::create(#_expr_, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); } }
#define JE_verify(_call_, ...) { const auto __ret_val__ = _call_; JE_assert(__ret_val__, __VA_ARGS__); }
#else
#define JE_assert(_value_, ...)
#define JE_verify(_call_, ...) _call_
#endif

#define JE_fail(...) JE_assert(false, __VA_ARGS__)
#define JE_assert_bailout(_value_, _return_type_, ...) { JE_assert(_value_, __VA_ARGS__); if(!(_value_)) { return _return_type_; } }
#define JE_todo() JE_fail("Not implemented.")