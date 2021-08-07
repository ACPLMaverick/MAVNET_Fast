#include "texture_vulkan.h"
#include "device_vulkan.h"

namespace je { namespace draw { namespace gpu {

    texture_vulkan::texture_vulkan(const texture_params& a_params)
        : texture(a_params)
        , m_image(VK_NULL_HANDLE)
        , m_view(VK_NULL_HANDLE)
    {

    }

    texture_vulkan::texture_vulkan(math::screen_size a_size, texture_format a_format, VkImage a_image)
        : texture(a_size, a_format)
        , m_image(a_image)
    {
    }

    texture_vulkan::~texture_vulkan()
    {
    }

    void texture_vulkan::shutdown(device& a_device)
    {
        shutdown_view(a_device);

    }

    bool texture_vulkan::init_internal(device& a_device, const texture_params& a_params)
    {
        JE_todo();
        return false;
    }

    bool texture_vulkan::clear_internal(device& a_device, math::color a_clear_color)
    {
        JE_todo();
        return false;
    }

    bool texture_vulkan::fill_with_buffer_data_internal(device& a_device, const data::array<u8>& a_buffer)
    {
        JE_todo();
        return false;
    }

    bool texture_vulkan::init_view(device& a_device, bool a_is_mipmapped)
    {
        device_vulkan& device = JE_vk_device(a_device);

        VkImageViewCreateInfo image_view_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        image_view_info.image = m_image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = texture_format_vk::to(m_format);
        image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = a_is_mipmapped ? compute_num_mips_from_size() : 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        JE_vk_verify_bailout(vkCreateImageView(device.get_device(), &image_view_info, device.get_allocator(), &m_view));
        return true;
    }

    void texture_vulkan::shutdown_view(device& a_device)
    {
        if(m_view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(JE_vk_device(a_device).get_device(), m_view, JE_vk_device(a_device).get_allocator());
            m_view = VK_NULL_HANDLE;
        }
    }

}}}