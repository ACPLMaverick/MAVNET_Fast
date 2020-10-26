#pragma once

#include <cstdlib>

#if JE_ASSERTIONS

// Not supported for now on Linux, at least until I get a window implementation.
#define JE_ASSERTION_USES_MESSAGE_BOX (1 && (JE_PLATFORM_WINDOWS))

namespace je { namespace util {

    class assertion
    {
    public:
        static void create(const char* expression, const char* file, const char* function, size_t line, const char* message, ...);
        static void create(const char* expression, const char* file, const char* function, size_t line);

        template<typename ptr_type> static ptr_type* check_ptr(ptr_type* ptr);
    };

}}

#endif

#if JE_ASSERTIONS
#define JE_assert(_expr_, ...) { if(!(_expr_)) { je::util::assertion::create(#_expr_, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); } }
#define JE_verify(_expr_, ...) JE_assert(_expr_, ##__VA_ARGS__)
#define JE_check(_ptr_) je::util::assertion::check_ptr(_ptr_)
#else
#define JE_assert(_value_, ...)
#define JE_verify(_call_, ...) (_call_)
#define JE_check(_ptr_) (_ptr_)
#endif

#define JE_fail(...) JE_assert(false, __VA_ARGS__)
#define JE_assert_bailout(_value_, _return_type_, ...) { JE_assert(_value_, __VA_ARGS__); if(!(_value_)) { return _return_type_; } }
#define JE_todo() JE_fail("Not implemented.")
#define JE_deref(_ptr_) *(JE_check(_ptr_))

#if JE_ASSERTIONS
template<typename ptr_type> ptr_type* je::util::assertion::check_ptr(ptr_type* ptr)
{
    if(ptr == nullptr)
    {
        JE_fail("Null pointer detected.");
    }
    return ptr;
}
#endif