#pragma once

#include "draw/gpu/texture.h"
#include "global_vulkan.h"

namespace je { namespace draw { namespace gpu {

    class texture_vulkan : public texture
    {
    public:

        VkImage get_image() const { return m_image; }
        VkImageView get_view() const { return m_view; }

    protected:

        texture_vulkan(const texture_params& params);
        texture_vulkan(math::screen_size size, texture_format format, VkImage image);
        ~texture_vulkan();

        void shutdown(device& a_device) override;
        bool init_internal(device& a_device, const texture_params& params) override;
        bool clear_internal(device& a_device, math::color clear_color) override;
        bool fill_with_buffer_data_internal(device& a_device, const data::array<u8>& buffer) override;

        bool init_view(device& a_device, bool is_mipmapped);
        void shutdown_view(device& a_device);

    protected:

        VkImage m_image;
        VkImageView m_view;

        friend class factory_vulkan;
        friend class presenter_vulkan;

    };

}}}