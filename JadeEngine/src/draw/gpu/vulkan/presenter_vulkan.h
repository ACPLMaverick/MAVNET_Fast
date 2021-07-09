#pragma once

#include "draw/gpu/presenter.h"
#include "global_vulkan.h"

namespace je { namespace draw {

    class gpu_vulkan;

    class presenter_vulkan : public presenter
    {
    public:

        ~presenter_vulkan();

        // Presenter interface.
        void shutdown(gpu& a_gpu) override;
        bool present(gpu& a_gpu/*TODO params : presented render target. Window position offset.*/) override;
        bool set_vsync(bool is_vsync) override;
        bool set_hdr(bool is_hdr) override;
        bool recreate(gpu& a_gpu, const window::window& updated_window) override;
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
        bool init(gpu& a_gpu, const presenter_params& params) override;
        // ///////////////
        bool init_surface_platform_specific(gpu_vulkan& gpu, const window::window& a_window);
        bool is_presenting_supported_by_graphics_queue(gpu_vulkan& gpu);
        bool init_swapchain_and_adjust_params(gpu_vulkan& gpu, const window::window& a_window);
        bool init_swapchain_images(gpu_vulkan& gpu);

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

        friend class gpu;
    };

}}