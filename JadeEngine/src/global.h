#pragma once

// JadeEngine global header.

// Global defines and utilities.

// TODO: Make this compiler-agnostic:
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

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
#define JE_print(_text_, ...) printf(_text_, ##__VA_ARGS__)
#define JE_print_ln(_text_, ...) JE_print(_text_, ##__VA_ARGS__); JE_print("\n")
#define JE_print_flush() fflush(stdout)
#else
#define JE_print(_text_, ...)
#define JE_print_ln(_text_, ...)
#define JE_print_flush()
#endif

// ///////////////////////

// Compile-time configurations.

#if JE_CONFIG_DEBUG
#define JE_DEBUG_ALLOCATIONS 1
#define JE_ASSERTIONS 1
#else
#define JE_DEBUG_ALLOCATIONS 0
#define JE_ASSERTIONS 0
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

#include "platform/linux/global.h"
#include "platform/windows/global.h"

// ///////////////////////