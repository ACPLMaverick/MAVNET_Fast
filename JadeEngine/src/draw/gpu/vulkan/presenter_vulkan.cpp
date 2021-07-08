#include "presenter_vulkan.h"
#include "window/window.h"
#include "gpu_vulkan.h"

namespace je { namespace draw {

    presenter_vulkan::~presenter_vulkan()
    {
    }

    void presenter_vulkan::shutdown(gpu& a_gpu)
    {
        if(m_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(JE_vk_gpu(a_gpu).get_instance(), m_surface, JE_vk_gpu(a_gpu).get_allocator());
            m_surface = VK_NULL_HANDLE;
        }
    }

    bool presenter_vulkan::present(gpu& a_gpu)
    {
        JE_todo();
        return false;
    }

    bool presenter_vulkan::set_vsync(bool is_vsync)
    {
        JE_todo();
        return false;
    }
    
    bool presenter_vulkan::set_hdr(bool is_hdr)
    {
        JE_todo();
        return false;
    }

    presenter_vulkan::presenter_vulkan(const presenter_params& a_params)
        : presenter(a_params)
        , m_surface(VK_NULL_HANDLE)
    {
    }

    bool presenter_vulkan::init(gpu& a_gpu, const window::window& a_window)
    {
        JE_verify_bailout(init_surface_platform_specific(JE_vk_gpu(a_gpu), a_window), false, "Failed to create a surface.");
        JE_verify_bailout(JE_vk_gpu(a_gpu).is_presenting_supported_by_graphics_queue(m_surface), false, "Presenting is not supported by the graphics queue");
        JE_verify_bailout(init_swapchain(JE_vk_gpu(a_gpu), a_window), false, "Failed to init swapchain.");
        JE_verify_bailout(init_swapchain_images(JE_vk_gpu(a_gpu)), false, "Failed to obtain swapchain images.");
        return true;
    }

    bool presenter_vulkan::init_surface_platform_specific(gpu_vulkan& a_gpu, const window::window& a_window)
    {
#if JE_PLATFORM_LINUX
        VkXcbSurfaceCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR };
        create_info.window = a_window.get_window();
        create_info.connection = a_window.get_connection();
        create_info.flags = 0;

        JE_vk_verify_bailout(vkCreateXcbSurfaceKHR(a_gpu.get_instance(), &create_info, a_gpu.get_allocator(), &m_surface));

#elif JE_PLATFORM_WINDOWS
#error TODO Implement.
#elif JE_PLATFORM_ANDROID
#error TODO Implement.
#else
#error TODO Implement.
#endif // JE_PLATFORM_LINUX

        return true;
    }

    bool presenter_vulkan::init_swapchain(gpu_vulkan& gpu, const window::window& a_window)
    {
        JE_todo();
        return false;
    }

    bool presenter_vulkan::init_swapchain_images(gpu_vulkan& gpu)
    {
        JE_todo();
        return false;
    }

}}