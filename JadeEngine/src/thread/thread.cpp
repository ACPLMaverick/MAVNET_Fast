#include "thread.h"

namespace je { namespace thread {

    void thread::sleep_us(u64 a_us)
    {
        sleep_ns(a_us * 1'000ULL);
    }

    void thread::sleep_ms(u64 a_ms)
    {
        sleep_ns(a_ms * 1'000'000ULL);
    }

    void thread::sleep_ms(f32 a_ms)
    {
        sleep_ns(static_cast<u64>(a_ms * static_cast<float>(1'000'000ULL)));
    }

    void thread::sleep(f32 sec)
    {
        sleep_ns(static_cast<u64>(sec * static_cast<float>(1'000'000'000ULL)));
    }

}}