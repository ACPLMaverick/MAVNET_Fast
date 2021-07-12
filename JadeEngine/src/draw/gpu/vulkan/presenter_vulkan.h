#pragma once

#include "draw/gpu/presenter.h"
#include "global_vulkan.h"

namespace je { namespace draw { namespace gpu {

    class dev_vulkan;

    class presenter_vulkan : public presenter
    {
    public:

        ~presenter_vulkan();

        // Presenter interface.
        void shutdown(dev& a_dev) override;
        bool present(dev& a_dev/*TODO params : presented render target. Window position offset.*/) override;
        bool set_vsync(bool is_vsync) override;
        bool set_hdr(bool is_hdr) override;
        bool recreate(dev& a_dev, const window::window& updated_window) override;
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

        // Presenter protected interface.
        bool init(dev& a_dev, const presenter_params& params) override;
        // ///////////////
        bool init_surface_platform_specific(dev_vulkan& dev, const window::window& a_window);
        bool is_presenting_supported_by_graphics_queue(dev_vulkan& dev);
        bool init_swapchain_and_adjust_params(dev_vulkan& dev, const window::window& a_window);
        bool init_swapchain_images(dev_vulkan& dev);

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

        friend class dev;
    };

}}}