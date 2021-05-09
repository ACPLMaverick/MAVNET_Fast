#pragma once

#include "global.h"

namespace je { namespace thread {

    class thread
    {
    public:

        static void sleep_ns(u64 ns);
        static void sleep_us(u64 us);
        static void sleep_ms(u64 ms);
        static void sleep_ms(f32 ms);
        static void sleep(f32 sec);

    };

}}