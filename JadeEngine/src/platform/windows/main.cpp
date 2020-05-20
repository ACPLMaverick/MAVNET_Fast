#include "global.h"

#include "mem/system_allocator.h"

#include <iostream>

namespace JE_NAMESPACE {

    void testConfig()
    {
    #if JE_CONFIG_DEBUG
        std::cout << "Hello world! Debug.";
    #elif JE_CONFIG_PROFILE
        std::cout << "Hello world! Profile.";
    #elif JE_CONFIG_RELEASE
        std::cout << "Hello world! Release.";
    #else
    #error "Configuration not specified."
    #endif
    }

    void testAlloc()
    {
        mem::system_allocator allocator;
        void* mem = allocator.allocate(64);
        allocator.free(mem);
    }

}

int main()
{
    //testConfig();
    je::testAlloc();
    return 0;
}