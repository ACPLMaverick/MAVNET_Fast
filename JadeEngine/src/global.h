#pragma once

// JadeEngine global header.

// Global defines.

#define JE_NAMESPACE je

// ///////////////////////

// Common includes.

#include <cstdlib>
#include <cstdint>

// ///////////////////////

// Debug print.

#if JE_CONFIG_RETAIL == 0
#define JE_printf(_text_, ...) printf(_text_, __VA_ARGS__);
#define JE_printf_ln(_text_, ...) JE_printf(_text_ "\n", __VA_ARGS__);
#else
#define JE_printf(_text_, ...)
#define JE_printf_ln(_text_, ...)
#endif

// Assertions.

#if JE_CONFIG_DEBUG
#include <assert.h>
// TODO make message visible.
#define JE_assert(_value_, ...) assert(_value_)
#define JE_verify(_call_, ...) { const auto val = _call_; JE_assert(val, __VA_ARGS__); }
#else
#define JE_assert(_value_, ...)
#define JE_verify(_call_, ...) _call_
#endif

#define JE_fail(...) JE_assert(false, __VA_ARGS__)

// ///////////////////////