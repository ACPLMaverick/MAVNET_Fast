#pragma once

#include "global.h"

#if JE_PLATFORM_LINUX
#define VK_USE_PLATFORM_XCB_KHR
#elif JE_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif JE_PLATFORM_ANDROID
#error TODO Implement.
#else
#error TODO Implement.
#endif

#include <vulkan/vulkan.h>

namespace je { namespace draw {

    // Utility classes.

    template<typename function_type>
    class vk_function_retriever
    {
    public:
        template<typename... args>
        static VkResult call(const char* a_func_name, VkInstance a_instance, args... a_args)
        {
            static PFN_vkVoidFunction func = vkGetInstanceProcAddr(a_instance, a_func_name);
            JE_assert(func != nullptr);
            return reinterpret_cast<function_type>(func)(a_instance, a_args...);
        }

        template<typename... args>
        static void call_noret(const char* a_func_name, VkInstance a_instance, args... a_args)
        {
            static PFN_vkVoidFunction func = vkGetInstanceProcAddr(a_instance, a_func_name);
            JE_assert(func != nullptr);
            reinterpret_cast<function_type>(func)(a_instance, a_args...);
        }
    };

    // ////////////////

}}

#define JE_vk_ext_func(_func_, _instance_, ...) je::draw::vk_function_retriever<PFN_##_func_>::call(#_func_, _instance_, __VA_ARGS__);
#define JE_vk_ext_func_noret(_func_, _instance_, ...) je::draw::vk_function_retriever<PFN_##_func_>::call_noret(#_func_, _instance_, __VA_ARGS__);
#define JE_vk_verify_bailout(_expression_)  \
{                                           \
    VkResult result = _expression_;         \
    JE_assert_bailout(result == VK_SUCCESS, false, "Vulkan function failed: [%s]", #_expression_);\
}
#define JE_vk_gpu(_gpu_) (reinterpret_cast<gpu_vulkan&>(_gpu_))