#pragma once

// JadeEngine global header.

// Global defines and utilities.

#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define JE_unused(_var_) (void)(_var_)
#define JE_disallow_copy(_type_)			    \
    _type_(const _type_&) = delete;				\
    _type_& operator=(const _type_&) = delete;
#define JE_safe_delete(_ptr_)	    \
    if (_ptr_ != nullptr)			\
    {								\
        delete _ptr_;				\
        _ptr_ = nullptr;			\
    }

#define JE_bitfield_enum_begin(_enum_, _derived_)   \
enum class _enum_ : _derived_                       \
{

#define JE_bitfield_enum_end(_enum_, _derived_)                             \
};                                                                          \
inline bool operator&(_enum_ a, _enum_ b)                                   \
{                                                                           \
    return static_cast<_derived_>(a) & static_cast<_derived_>(b);           \
}                                                                           \
inline bool operator|(_enum_ a, _enum_ b)                                   \
{                                                                           \
    return static_cast<_derived_>(a) | static_cast<_derived_>(b);           \
}


// ///////////////////////

// Common includes.

#include <cstdlib>
#include <cstdint>
#include <cstring>

#if JE_CONFIG_DEBUG
#include <cstdio>
#endif

// ///////////////////////

// Debug print.

#if JE_CONFIG_DEBUG
#define JE_printf(_text_, ...) printf(_text_, __VA_ARGS__);
#define JE_printf_ln(_text_, ...) JE_printf(_text_, __VA_ARGS__); printf("\n")
#else
#define JE_printf(_text_, ...)
#define JE_printf_ln(_text_, ...)
#endif

// Assertions.

#if JE_CONFIG_DEBUG
#include <assert.h>
// TODO make message visible.
#define JE_assert(_value_, ...) { if(!(_value_)) { JE_printf_ln(__VA_ARGS__); } assert(_value_); }
#define JE_verify(_call_, ...) { const auto val = _call_; JE_assert(val, __VA_ARGS__); }
#else
#define JE_assert(_value_, ...)
#define JE_verify(_call_, ...) _call_
#endif

#define JE_fail(...) JE_assert(false, __VA_ARGS__)
#define JE_assert_bailout(_value_, _return_type_, ...) { JE_assert(_value_, __VA_ARGS__); if(!(_value_)) { return _return_type_; } }
#define JE_todo() JE_fail("Not implemented.")

// ///////////////////////