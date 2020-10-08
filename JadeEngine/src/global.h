#pragma once

// JadeEngine global header.

// Global defines and utilities.

#define JE_unused(_var_) (void)(_var_)
#define JE_disallow_copy(_type_)			    \
    _type_(const _type_&) = delete;				\
    _type_& operator=(const _type_&) = delete
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
#define JE_print(_text_) printf(_text_)
#define JE_print_ln(_text_) JE_print(_text_); JE_print("\n")
#define JE_printf(_text_, ...) printf(_text_, __VA_ARGS__)
#define JE_printf_ln(_text_, ...) JE_printf(_text_, __VA_ARGS__); JE_print("\n")
#define JE_flush_stdout() fflush(stdout)
#else
#define JE_print(_text_)
#define JE_print_ln(_text_) 
#define JE_printf(_text_, ...)
#define JE_printf_ln(_text_, ...)
#define JE_flush_stdout()
#endif

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

#include "util/assertion.h"
#include "data/data.h"
// TODO: thread/thread.h

// ///////////////////////

// Platform-specific project-wide includes.

#include "platf/linux/global_linux.h"

// ///////////////////////