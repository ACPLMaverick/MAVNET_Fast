#pragma once

#include "draw/gpu/presenter.h"
#include "global_vulkan.h"

namespace je { namespace draw { namespace gpu {

    class device;
    class device_vulkan;

    class presenter_vulkan : public presenter
    {
    public:

        // Presenter interface.
        bool present(device& a_device/*TODO params : presented render target. Window position offset.*/) override;
        bool set_vsync(bool is_vsync) override;
        bool set_hdr(bool is_hdr) override;
        bool recreate(device& a_device, const window::window& updated_window) override;
        // ////////////////

    protected:

        struct image_format
        {
            VkFormat m_format;
            VkColorSpaceKHR m_color_space;

            image_format(VkFormat a_format, VkColorSpaceKHR a_color_space)
                : m_format(a_format)
                , m_color_space(a_color_space)
            {
            }

            bool operator==(const image_format& other) const
            {
                return m_format == other.m_format && m_color_space == other.m_color_space;
            }
        };

        struct swapchain_image
        {
            VkImage m_image;
        };

    protected:

        presenter_vulkan(const presenter_params& params);
        ~presenter_vulkan();

        // Presenter protected interface.
        bool init(device& a_device, const presenter_params& params) override;
        void shutdown(device& a_device) override;
        // ///////////////
        bool init_surface_platform_specific(device_vulkan& device, const window::window& a_window);
        bool is_presenting_supported_by_graphics_queue(device_vulkan& device);
        bool init_swapchain_and_adjust_params(device_vulkan& device, const window::window& a_window);
        bool init_swapchain_images(device_vulkan& device);

        image_format get_selected_image_format() const;
        VkPresentModeKHR get_selected_present_mode() const;

    protected:

        static const image_format k_format_sdr;
        static const image_format k_format_hdr;
        static const VkPresentModeKHR k_present_mode_immediate;
        static const VkPresentModeKHR k_present_mode_vsync;

        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapchain;
        VkSwapchainKHR m_old_swapchain;
        data::array<swapchain_image> m_images;

    protected:

        friend class factory;
    };

}}}