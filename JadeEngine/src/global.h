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

#define JE_bitfield enum

// ///////////////////////

// Common includes.

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <type_traits>

#if JE_CONFIG_DEBUG
#include <cstdio>
#endif

// ///////////////////////

// Debug print.

#if JE_CONFIG_DEBUG
#define JE_printf(_text_, ...) printf(_text_, __VA_ARGS__)
#define JE_printf_ln(_text_, ...) JE_printf(_text_, __VA_ARGS__); printf("\n")
#else
#define JE_printf(_text_, ...)
#define JE_printf_ln(_text_, ...)
#endif

// Assertions.

#if JE_CONFIG_DEBUG
#if JE_PLATFORM_WINDOWS
#define JE_debugbreak __debugbreak
#elif JE_PLATFORM_LINUX || JE_PLATFORM_ANDROID
#define JE_debugbreak raise(SIGTRAP)
#endif
// TODO make message visible.
#define JE_assert(_value_, ...) { auto __ret__ = (_value_); if(!(__ret__)) { JE_printf("ASSERT on expression [%s] : ", #_value_); JE_printf_ln(__VA_ARGS__); JE_debugbreak(); } }
#define JE_verify(_call_, ...) { const auto __ret_val__ = _call_; JE_assert(__ret_val__, __VA_ARGS__); }
#else
#define JE_assert(_value_, ...)
#define JE_verify(_call_, ...) _call_
#endif

#define JE_fail(...) JE_assert(false, __VA_ARGS__)
#define JE_assert_bailout(_value_, _return_type_, ...) { JE_assert(_value_, __VA_ARGS__); if(!(_value_)) { return _return_type_; } }
#define JE_todo() JE_fail("Not implemented.")

// ///////////////////////

// Compile-time configurations.

#if JE_CONFIG_DEBUG
#define JE_DEBUG_ALLOCATIONS 1
#else
#define JE_DEBUG_ALLOCATIONS 0
#endif

#define JE_DATA_STRUCTS_STD_BACKEND 1

#if JE_DATA_STRUCTS_STD_BACKEND
#define JE_DATA_STRUCTS_STD_BACKEND_CUSTOM_ALLOCATOR 1
#endif

// ///////////////////////

// Project-wide includes.

#include "data/data.h"
// TODO: thread/thread.h

// ///////////////////////