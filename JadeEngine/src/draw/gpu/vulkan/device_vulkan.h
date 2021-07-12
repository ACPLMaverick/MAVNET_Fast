#pragma once

#include "draw/gpu/device.h"
#include "global_vulkan.h"

namespace je { namespace draw { namespace gpu {

    class device_vulkan : public device
    {
    public:

        struct vk_queue_families
        {
            i32 family_indices[static_cast<size>(queue_type::k_enum_size)];
        };

    protected:

        enum class vk_extension_layer_requirement_level : u8
        {
            k_mandatory = 0,    // Should return false if not available.
            k_required = 1,     // Should emit a warning but proceed if not available.
            k_optional = 2      // Should do nothing if not available.
        };

        struct vk_extension_layer_definition
        {
            const char* m_name;
            vk_extension_layer_requirement_level m_requirement;
            capabilities m_cap_flag_to_set = capabilities::k_none;
        };

    public:

        // GPU interface.

        // ///////////////

        // Accessor functions.
        inline VkInstance get_instance() { return m_instance; }
        inline VkPhysicalDevice get_physical_device() { return m_physical_device; }
        inline VkDevice get_device() { return m_device; }
        inline vk_queue_families get_queue_families() { return m_queue_families; }
        inline VkQueue get_queue(queue_type a_type) { return m_queues[static_cast<size>(a_type)]; }
        inline VkAllocationCallbacks* get_allocator() { return nullptr; } // TODO
        // //////////////////

        // Utility/helper functions.
        
        // //////////////////

    protected:

        device_vulkan(const device_params& initializer);
        ~device_vulkan();

        bool init(const device_params& initializer) override;
        void shutdown() override;
        const char* get_name() override { return "Vulkan"; }

    protected:

        // Initialization functions.
        bool init_instance();
        bool init_instance_filter_extensions_and_layers
        (
            const vk_extension_layer_definition* wanted_extensions,
            const size wanted_extensions_num,
            const vk_extension_layer_definition* wanted_layers,
            const size wanted_layers_num,
            data::array<const char*>& out_extensions,
            data::array<const char*>& out_layers
        );
        void init_instance_debug_output();
        bool init_physical_device(i8 forced_adapter_index, VkPhysicalDeviceFeatures2& out_features, vk_queue_families& out_queue_families);
        // Returns false if the device is not appropriate for usage.
        bool init_physical_device_get_info_for_adapter(VkPhysicalDevice device, capabilities& out_caps,
            VkPhysicalDeviceFeatures2& out_features, vk_queue_families& out_queue_families);
        bool init_device(const VkPhysicalDeviceFeatures2& features, const vk_queue_families& queue_families);
        void init_queues(const vk_queue_families& queue_families);

        // Debug-callback related.
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_static
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
            void* user_data
        );
        void debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const char* message);

    protected:

        VkInstance m_instance;
#if JE_GPU_DEBUG_LAYERS
        VkDebugUtilsMessengerEXT m_debug;
#endif // JE_GPU_DEBUG_LAYERS
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        data::static_array<VkQueue, static_cast<size>(queue_type::k_enum_size)> m_queues;
        vk_queue_families m_queue_families;

    protected:

        friend class factory;
    };

}}}