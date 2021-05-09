#include "thread/thread.h"

#if JE_PLATFORM_LINUX

namespace je { namespace thread {

    void thread::sleep_ns(u64 a_ns)
    {
        static const u64 k_one_second_ns = 1'000'000'000ULL;
        timespec ts;
        if(a_ns > k_one_second_ns)
        {
            ts.tv_sec = a_ns / k_one_second_ns;
            ts.tv_nsec = a_ns % k_one_second_ns;
        }
        else
        {
            ts.tv_sec = 0;
            ts.tv_nsec = a_ns;
        }
        nanosleep(&ts, nullptr);
    }

}}

#endif // JE_PLATFORM_LINUX