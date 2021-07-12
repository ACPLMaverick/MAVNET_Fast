#include "presenter_vulkan.h"
#include "window/window.h"
#include "dev_vulkan.h"
#include "math/sc.h"

namespace je { namespace draw { namespace gpu {

    presenter_vulkan::~presenter_vulkan()
    {
    }

    void presenter_vulkan::shutdown(dev& a_dev)
    {
        if(m_old_swapchain != VK_NULL_HANDLE)
        {
            m_old_swapchain = VK_NULL_HANDLE;
        }
        if(m_swapchain != VK_NULL_HANDLE)
        {
            m_images.clear();
            vkDestroySwapchainKHR(JE_vk_dev(a_dev).get_device(), m_swapchain, JE_vk_dev(a_dev).get_allocator());
        }
        if(m_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(JE_vk_dev(a_dev).get_instance(), m_surface, JE_vk_dev(a_dev).get_allocator());
            m_surface = VK_NULL_HANDLE;
        }
    }

    bool presenter_vulkan::present(dev& a_dev)
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

    bool presenter_vulkan::recreate(dev& a_dev, const window::window& a_updated_window)
    {
        m_old_swapchain = m_swapchain;
        m_images.clear();
        JE_verify_bailout(init_swapchain_and_adjust_params(JE_vk_dev(a_dev), a_updated_window), false, "Failed to init swapchain.");
        JE_verify_bailout(init_swapchain_images(JE_vk_dev(a_dev)), false, "Failed to obtain swapchain images.");
        return true;
    }

    presenter_vulkan::presenter_vulkan(const presenter_params& a_params)
        : presenter(a_params)
        , m_surface(VK_NULL_HANDLE)
        , m_swapchain(VK_NULL_HANDLE)
        , m_old_swapchain(VK_NULL_HANDLE)
    {
    }

    bool presenter_vulkan::init(dev& a_dev, const presenter_params& a_params)
    {
        JE_verify_bailout(init_surface_platform_specific(JE_vk_dev(a_dev), a_params.m_window), false, "Failed to create a surface.");
        JE_verify_bailout(is_presenting_supported_by_graphics_queue(JE_vk_dev(a_dev)), false, "Presenting is not supported by the graphics queue");
        JE_verify_bailout(init_swapchain_and_adjust_params(JE_vk_dev(a_dev), a_params.m_window), false, "Failed to init swapchain.");
        JE_verify_bailout(init_swapchain_images(JE_vk_dev(a_dev)), false, "Failed to obtain swapchain images.");
        return true;
    }

    bool presenter_vulkan::init_surface_platform_specific(dev_vulkan& a_dev, const window::window& a_window)
    {
#if JE_PLATFORM_LINUX
        VkXcbSurfaceCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR };
        create_info.window = a_window.get_window();
        create_info.connection = a_window.get_connection();
        create_info.flags = 0;

        JE_vk_verify_bailout(vkCreateXcbSurfaceKHR(a_dev.get_instance(), &create_info, a_dev.get_allocator(), &m_surface));

#elif JE_PLATFORM_WINDOWS
#error TODO Implement.
#elif JE_PLATFORM_ANDROID
#error TODO Implement.
#else
#error TODO Implement.
#endif // JE_PLATFORM_LINUX

        return true;
    }

    bool presenter_vulkan::is_presenting_supported_by_graphics_queue(dev_vulkan& a_dev)
    {
        if(a_dev.get_queue_families().family_indices[static_cast<size>(queue_type::k_graphics)] >= 0)
        {
            VkBool32 is_present_supported = false;
            JE_vk_verify_bailout(vkGetPhysicalDeviceSurfaceSupportKHR(
                a_dev.get_physical_device(),
                a_dev.get_queue_families().family_indices[static_cast<size>(queue_type::k_graphics)],
                m_surface, &is_present_supported));
            return static_cast<bool>(is_present_supported);
        }
        return false;
    }

    bool presenter_vulkan::init_swapchain_and_adjust_params(dev_vulkan& a_dev, const window::window& a_window)
    {
        dev_vulkan& dev = JE_vk_dev(a_dev);

        // Query swapchain details.
        const VkPhysicalDeviceSurfaceInfo2KHR info
        {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
            nullptr,
            m_surface
        };
        VkSurfaceCapabilities2KHR caps{ VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR };
        JE_vk_verify_bailout(vkGetPhysicalDeviceSurfaceCapabilities2KHR(dev.get_physical_device(), &info, &caps));

        u32 format_num = 0;
        JE_vk_verify_bailout(vkGetPhysicalDeviceSurfaceFormats2KHR(dev.get_physical_device(), &info, &format_num, nullptr));

        data::array<VkSurfaceFormat2KHR> surface_formats(format_num);
        if(format_num > 0)
        {
            for(u32 i = 0; i < format_num; ++i)
            {
                surface_formats[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
            }
            JE_vk_verify_bailout(vkGetPhysicalDeviceSurfaceFormats2KHR(dev.get_physical_device(), &info, &format_num, surface_formats.data()));
        }
        else
        {
            return false;
        }

        u32 present_mode_num = 0;
        JE_vk_verify_bailout(vkGetPhysicalDeviceSurfacePresentModesKHR(dev.get_physical_device(), m_surface, &present_mode_num, nullptr));
        data::array<VkPresentModeKHR> present_modes(present_mode_num);
        if(present_mode_num > 0)
        {
            JE_vk_verify_bailout(vkGetPhysicalDeviceSurfacePresentModesKHR(dev.get_physical_device(), m_surface, &present_mode_num, present_modes.data()));
        }
        else
        {
            return false;
        }

        // Adjust caps and required parameters that are given in init.
        if(caps.surfaceCapabilities.maxImageCount >= 3)
        {
            reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(capabilities::k_triple_buffering);
        }

        m_backbuffer_width = math::sc::clamp(m_backbuffer_width, (u16)caps.surfaceCapabilities.minImageExtent.width, (u16)caps.surfaceCapabilities.maxImageExtent.width);
        m_backbuffer_height = math::sc::clamp(m_backbuffer_height, (u16)caps.surfaceCapabilities.minImageExtent.height, (u16)caps.surfaceCapabilities.maxImageExtent.height);
        m_num_buffers = math::sc::clamp(m_num_buffers, (u8)caps.surfaceCapabilities.minImageCount, (u8)caps.surfaceCapabilities.maxImageCount);

        bool is_sdr_format_available = false;
        bool is_hdr_format_available = false;
        for(const VkSurfaceFormat2KHR& surface_format : surface_formats)
        {
            const image_format format(surface_format.surfaceFormat.format, surface_format.surfaceFormat.colorSpace);
            is_sdr_format_available |= format == k_format_sdr;
            is_hdr_format_available |= format == k_format_hdr;

            if(is_sdr_format_available && is_hdr_format_available)
            {
                break;
            }
        }

        if(is_sdr_format_available == false)
        {
            return false;
        }

        if(is_hdr_format_available)
        {
            reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(capabilities::k_hdr);
        }

        for(const VkPresentModeKHR mode : present_modes)
        {
            if(mode == k_present_mode_immediate)
            {
                reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(capabilities::k_immediate);
            }
            if(mode == k_present_mode_vsync)
            {
                reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(capabilities::k_vsync);
            }
        }

        if(has_capabilities(capabilities::k_vsync) == false && m_is_vsync)
        {
            JE_print("WARNING. Presenter : Vsync specified but not supported.");
            m_is_vsync = false;
        }
        else if(has_capabilities(capabilities::k_vsync) && has_capabilities(capabilities::k_immediate) == false && m_is_vsync == false)
        {
            JE_print("WARNING. Presenter : Immediate mode specified but not supported.");
            m_is_vsync = true;
        }
        else if(has_capabilities(capabilities::k_vsync) == false && has_capabilities(capabilities::k_immediate) == false)
        {
            return false;
        }

        if(has_capabilities(capabilities::k_hdr) == false && m_is_hdr)
        {
            JE_print("WARNING. Presenter : HDR specified but not supported.");
            m_is_hdr = false;
        }

        // Create swapchain itself.
        const image_format selected_image_format = get_selected_image_format();

        VkSwapchainCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        create_info.surface = m_surface;
        create_info.minImageCount = m_num_buffers;
        create_info.imageFormat = selected_image_format.m_format;
        create_info.imageColorSpace = selected_image_format.m_color_space;
        create_info.imageExtent.width = m_backbuffer_width;
        create_info.imageExtent.height = m_backbuffer_height;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // TODO change this to TRANSFER_DST_BIT later on.
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
        create_info.preTransform = caps.surfaceCapabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = get_selected_present_mode();
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = m_old_swapchain;

        JE_vk_verify_bailout(vkCreateSwapchainKHR(dev.get_device(), &create_info, dev.get_allocator(), &m_swapchain));

        return true;
    }

    bool presenter_vulkan::init_swapchain_images(dev_vulkan& a_dev)
    {
        u32 images_num = 0;
        JE_vk_verify_bailout(vkGetSwapchainImagesKHR(JE_vk_dev(a_dev).get_device(), m_swapchain, &images_num, nullptr));
        JE_assert_bailout(images_num == m_num_buffers, false, "Swapchain image num differs from requested buffer num.");
        data::array<VkImage> images(images_num);
        JE_vk_verify_bailout(vkGetSwapchainImagesKHR(JE_vk_dev(a_dev).get_device(), m_swapchain, &images_num, images.data()));
        
        for(VkImage vk_image : images)
        {
            swapchain_image image;
            image.m_image = vk_image; 
            
            // Todo image view. -> these should be gpu::textures later on.
            
            // Todo synchronization elements.

            m_images.push_back(image);
        }

        return true;
    }

    presenter_vulkan::image_format presenter_vulkan::get_selected_image_format() const
    {
        return m_is_hdr ? k_format_hdr : k_format_sdr;
    }

    VkPresentModeKHR presenter_vulkan::get_selected_present_mode() const
    {
        return m_is_vsync ? k_present_mode_vsync : k_present_mode_immediate;
    }

    const presenter_vulkan::image_format presenter_vulkan::k_format_sdr( VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR );
    const presenter_vulkan::image_format presenter_vulkan::k_format_hdr( VK_FORMAT_B10G11R11_UFLOAT_PACK32, VK_COLOR_SPACE_HDR10_ST2084_EXT ); // TODO untested.
    const VkPresentModeKHR presenter_vulkan::k_present_mode_immediate = VK_PRESENT_MODE_IMMEDIATE_KHR;
    const VkPresentModeKHR presenter_vulkan::k_present_mode_vsync = VK_PRESENT_MODE_FIFO_KHR;

}}}