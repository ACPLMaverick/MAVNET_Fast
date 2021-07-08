#pragma once

#include <cstdint>

namespace je {

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using size = size_t;
    using uptr = uintptr_t;
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using f16 = u16;    // TODO float 16.
    using f32 = float;
    using f64 = double;

    static_assert(sizeof(u8) == 1);
    static_assert(sizeof(u16) == 2);
    static_assert(sizeof(u32) == 4);
    static_assert(sizeof(u64) == 8);
    static_assert(sizeof(size) == 8);
    static_assert(sizeof(uptr) == 8);
    static_assert(sizeof(i8) == 1);
    static_assert(sizeof(i16) == 2);
    static_assert(sizeof(i32) == 4);
    static_assert(sizeof(i64) == 8);
    static_assert(sizeof(f16) == 2);
    static_assert(sizeof(f32) == 4);
    static_assert(sizeof(f64) == 8);
    
}