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
        // ////////////////

    protected:

        struct swapchain_image
        {
            VkImage m_image;
        };

    protected:

        presenter_vulkan(const presenter_params& params);

        // Presenter protected interface.
        bool init(gpu& a_gpu, const window::window& a_window) override;
        // ///////////////
        bool init_surface_platform_specific(gpu_vulkan& gpu, const window::window& a_window);
        bool init_swapchain(gpu_vulkan& gpu, const window::window& a_window);
        bool init_swapchain_images(gpu_vulkan& gpu);

    protected:

        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapchain;
        data::array<swapchain_image> m_images;

    protected:

        friend class gpu_vulkan;
    };

}}