#include "gpu_vulkan.h"
#include "util/misc.h"
#include "presenter_vulkan.h"

namespace je { namespace draw {

    gpu_vulkan::~gpu_vulkan()
    {

    }

    gpu_vulkan::gpu_vulkan()
        : gpu()
        , m_instance(VK_NULL_HANDLE)
#if JE_GPU_DEBUG_LAYERS
        , m_debug(VK_NULL_HANDLE)
#endif // JE_GPU_DEBUG_LAYERS
        , m_physical_device(VK_NULL_HANDLE)
        , m_device(VK_NULL_HANDLE)
    {
        for(size i = 0; i < m_queues.k_num_objects; ++i)
        {
            m_queues[i] = VK_NULL_HANDLE;
        }
        for(size i = 0; i < static_cast<size>(queue_type::k_enum_size); ++i)
        {
            m_queue_families.family_indices[i] = -1;
        }
    }

    presenter* gpu_vulkan::create_presenter(const presenter_params& a_params)
    {
        presenter_vulkan* pres = new presenter_vulkan(a_params);
        if(pres->init(*this, a_params.m_window))
        {
            return pres;
        }
        else
        {
            return nullptr;
        }
    }

    bool gpu_vulkan::is_presenting_supported_by_graphics_queue(VkSurfaceKHR a_surface)
    {
        if(m_queue_families.family_indices[static_cast<size>(queue_type::k_graphics)] >= 0)
        {
            VkBool32 is_present_supported = false;
            JE_vk_verify_bailout(vkGetPhysicalDeviceSurfaceSupportKHR(
                m_physical_device, m_queue_families.family_indices[static_cast<size>(queue_type::k_graphics)],
                a_surface, &is_present_supported));
            return static_cast<bool>(is_present_supported);
        }
        return false;
    }

    bool gpu_vulkan::init(const gpu_params& a_initializer)
    {
        JE_verify_bailout(init_instance(), false, "Failed to init instance.");

        VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        vk_queue_families queue_families{};
        JE_verify_bailout(init_physical_device(a_initializer.m_forced_adapter_index, features, queue_families), false, "Failed to create a physical device.");
        JE_verify_bailout(init_device(features, queue_families), false, "Failed to create a logical device.");
        init_queues(queue_families);
        m_queue_families = queue_families;

        return true;
    }

    bool gpu_vulkan::shutdown()
    {
        for(size i = 0; i < m_queues.k_num_objects; ++i)
        {
            m_queues[i] = VK_NULL_HANDLE;
        }
        if(m_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_device, get_allocator());
            m_device = VK_NULL_HANDLE;
        }
        m_physical_device = VK_NULL_HANDLE;
#if JE_GPU_DEBUG_LAYERS
        if(m_debug != VK_NULL_HANDLE)
        {
            JE_vk_ext_func_noret(vkDestroyDebugUtilsMessengerEXT, m_instance, m_debug, get_allocator());
            m_debug = VK_NULL_HANDLE;
        }
#endif // JE_GPU_DEBUG_LAYERS
        if(m_instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_instance, get_allocator());
            m_instance = VK_NULL_HANDLE;
        }
        return true;
    }

    bool gpu_vulkan::init_instance()
    {
        // TODO fill with meaningful info.
        VkApplicationInfo app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app_info.pApplicationName = "JadeEngine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "JadeEngine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo create_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        create_info.pApplicationInfo = &app_info;

        static const vk_extension_layer_definition wanted_extensions[] = 
        {
            { "VK_KHR_display", vk_extension_layer_requirement_level::k_mandatory },
            { "VK_KHR_get_display_properties2", vk_extension_layer_requirement_level::k_mandatory },
            { "VK_KHR_get_physical_device_properties2", vk_extension_layer_requirement_level::k_mandatory },
            { "VK_KHR_get_surface_capabilities2", vk_extension_layer_requirement_level::k_mandatory },
            { "VK_KHR_surface", vk_extension_layer_requirement_level::k_mandatory },
#if JE_PLATFORM_LINUX
            { "VK_KHR_xcb_surface", vk_extension_layer_requirement_level::k_mandatory },
#elif JE_PLATFORM_WINDOWS
            { "VK_KHR_win32_surface", vk_extension_layer_requirement_level::k_mandatory },
#elif JE_PLATFORM_ANDROID
#error "TODO Implement"
#endif // JE_PLATFORM_LINUX
#if JE_GPU_DEBUG_LAYERS
            { "VK_EXT_debug_report", vk_extension_layer_requirement_level::k_required },
            { "VK_EXT_debug_utils", vk_extension_layer_requirement_level::k_required },
#endif // JE_GPU_DEBUG_LAYERS
        };

        static const vk_extension_layer_definition wanted_layers[] = 
        {
#if JE_GPU_DEBUG_LAYERS
            { "VK_LAYER_KHRONOS_validation", vk_extension_layer_requirement_level::k_required, capabilities::k_debug },
#endif // JE_GPU_DEBUG_LAYERS
#if JE_GPU_PROFILE_MARKERS
            { "VK_LAYER_LUNARG_monitor", vk_extension_layer_requirement_level::k_required },
#endif // JE_GPU_PROFILE_MARKERS
            { "VK_LAYER_LUNARG_screenshot", vk_extension_layer_requirement_level::k_required },
            { "VK_LAYER_NV_optimus", vk_extension_layer_requirement_level::k_optional, capabilities::k_optimus }
        };

        data::array<const char*> final_extensions;
        data::array<const char*> final_layers;
        if(init_instance_filter_extensions_and_layers(wanted_extensions, JE_array_num(wanted_extensions),
            wanted_layers, JE_array_num(wanted_layers), final_extensions, final_layers) == false)
        {
            return false;
        }

        create_info.enabledExtensionCount = final_extensions.size();
        create_info.ppEnabledExtensionNames = final_extensions.data();
        create_info.enabledLayerCount = final_layers.size();
        create_info.ppEnabledLayerNames = final_layers.data();

        JE_vk_verify_bailout(vkCreateInstance(&create_info, get_allocator(), &m_instance));

        init_instance_debug_output();

        return true;
    }

    bool gpu_vulkan::init_instance_filter_extensions_and_layers
    (
        const vk_extension_layer_definition* a_wanted_extensions,
        const size a_wanted_extensions_num,
        const vk_extension_layer_definition* a_wanted_layers,
        const size a_wanted_layers_num,
        data::array<const char*>& a_out_extensions,
        data::array<const char*>& a_out_layers
    )
    {
        u32 extension_num = 0;
        JE_vk_verify_bailout(vkEnumerateInstanceExtensionProperties(nullptr, &extension_num, nullptr));
        data::array<VkExtensionProperties> available_extensions(extension_num);
        if(extension_num > 0)
        {
            JE_vk_verify_bailout(vkEnumerateInstanceExtensionProperties(nullptr, &extension_num, available_extensions.data()));
        }

        u32 layer_num = 0;
        JE_vk_verify_bailout(vkEnumerateInstanceLayerProperties(&layer_num, nullptr));
        data::array<VkLayerProperties> available_layers(layer_num);
        if(layer_num > 0)
        {
            JE_vk_verify_bailout(vkEnumerateInstanceLayerProperties(&layer_num, available_layers.data()));
        }

        // Go through all wanted extensions/layers and add them to out array if applicable.
        // Raise proper error if layer was not found.
        for(size i = 0; i < a_wanted_extensions_num; ++i)
        {
            const vk_extension_layer_definition& extension_definition = a_wanted_extensions[i];
            const VkExtensionProperties* found_property = nullptr;
            for(const VkExtensionProperties& extension_property : available_extensions)
            {
                if(data::string::compare(extension_property.extensionName, extension_definition.m_name))
                {
                    found_property = &extension_property;
                    break;
                }
            }

            if(found_property == nullptr)
            {
                if(extension_definition.m_requirement == vk_extension_layer_requirement_level::k_mandatory)
                {
                    JE_fail("Extension [%s] mandatory but not supported.", extension_definition.m_name);
                    return false;
                }
                else if(extension_definition.m_requirement == vk_extension_layer_requirement_level::k_required)
                {
                    JE_print("WARNING. Extension [%s] was required but it is not available.", extension_definition.m_name);
                }
            }
            else
            {
                a_out_extensions.push_back(extension_definition.m_name);

                if(extension_definition.m_cap_flag_to_set != capabilities::k_none)
                {
                    reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(extension_definition.m_cap_flag_to_set);
                }
            }
        }

        for(size i = 0; i < a_wanted_layers_num; ++i)
        {
            const vk_extension_layer_definition& layer_definition = a_wanted_layers[i];
            const VkLayerProperties* found_property = nullptr;
            for(const VkLayerProperties& layer_property : available_layers)
            {
                if(data::string::compare(layer_property.layerName, layer_definition.m_name))
                {
                    found_property = &layer_property;
                    break;
                }
            }

            if(found_property == nullptr)
            {
                if(layer_definition.m_requirement == vk_extension_layer_requirement_level::k_mandatory)
                {
                    JE_fail("Layer [%s] mandatory but not supported.", layer_definition.m_name);
                    return false;
                }
                else if(layer_definition.m_requirement == vk_extension_layer_requirement_level::k_required)
                {
                    JE_print("WARNING. Layer [%s] was required but it is not available.", layer_definition.m_name);
                }
            }
            else
            {
                a_out_layers.push_back(layer_definition.m_name);

                if(layer_definition.m_cap_flag_to_set != capabilities::k_none)
                {
                    reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(layer_definition.m_cap_flag_to_set);
                }
            }
        }

        return true;
    }

    void gpu_vulkan::init_instance_debug_output()
    {
#if JE_GPU_DEBUG_LAYERS
        if(has_capabilities(capabilities::k_debug))
        {
            // TODO setup these based on config/initializer.
            VkDebugUtilsMessengerCreateInfoEXT create_info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
            create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            create_info.pfnUserCallback = &gpu_vulkan::debug_callback_static;
            create_info.pUserData = static_cast<void*>(this);

            VkResult result = JE_vk_ext_func(vkCreateDebugUtilsMessengerEXT, m_instance, &create_info, get_allocator(), &m_debug);
            if(result != VK_SUCCESS)
            {
                JE_print("WARNING. Failed to create Vulkan debug callback while having debug capability.");
            }
        }
#endif // JE_GPU_DEBUG_LAYERS
    }

    bool gpu_vulkan::init_physical_device(i8 a_forced_adapter_index, VkPhysicalDeviceFeatures2& a_out_features, vk_queue_families& a_out_queue_families)
    {
        m_physical_device = nullptr;

        u32 device_num = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_num, nullptr);
        if(device_num == 0)
        {
            JE_print("ERROR. Failed to find a physical device.");
            return false;
        }

        data::array<VkPhysicalDevice> devices(device_num);
        vkEnumeratePhysicalDevices(m_instance, &device_num, devices.data());

        if(a_forced_adapter_index >= 0 && a_forced_adapter_index < device_num)
        {
            // We can force-pick an adapter here.
            capabilities caps = capabilities::k_none;
            if(init_physical_device_get_info_for_adapter(devices[a_forced_adapter_index], caps, a_out_features, a_out_queue_families))
            {
                reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(caps);
                m_physical_device = devices[a_forced_adapter_index];
            }
        }
        else if(device_num == 1)
        {
            // No need for doing any fancy picking.
            capabilities caps = capabilities::k_none;
            if(init_physical_device_get_info_for_adapter(devices[0], caps, a_out_features, a_out_queue_families))
            {
                reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(caps);
                m_physical_device = devices[0];
            }
        }
        else
        {
            // Pick an adapter which has the most caps.
            struct sorted_data
            {
                VkPhysicalDeviceFeatures2 m_features;
                vk_queue_families m_queue_families;
                VkPhysicalDevice m_physical_device;
                capabilities m_caps;
                u8 m_caps_num_bits;

                sorted_data(const VkPhysicalDeviceFeatures2& a_features, const vk_queue_families& a_queue_families,
                    capabilities a_caps, VkPhysicalDevice a_device)
                    : m_features(a_features)
                    , m_queue_families(a_queue_families)
                    , m_physical_device(a_device)
                    , m_caps(a_caps)
                    , m_caps_num_bits(util::misc::get_num_bits(static_cast<u32>(m_caps)))
                {
                }

                bool operator>(const sorted_data& a_other) const
                {
                    return m_caps_num_bits > a_other.m_caps_num_bits;
                }
            };

            data::sorted_set<sorted_data, data::sort_greater<sorted_data>> ranking;
            for(VkPhysicalDevice device : devices)
            {
                VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
                vk_queue_families queue_families{};
                capabilities caps = capabilities::k_none;
                // Do not insert if the device is not appropriate (returns false).
                if(init_physical_device_get_info_for_adapter(device, caps, features, queue_families))
                {
                    ranking.emplace(features, queue_families, caps, device);
                }
            }

            if(ranking.size() > 0)
            {
                const sorted_data& selected = *(ranking.begin());
                reinterpret_cast<u32&>(m_capabilities) |= static_cast<u32>(selected.m_caps);
                a_out_features = selected.m_features;
                a_out_queue_families = selected.m_queue_families;
                m_physical_device = selected.m_physical_device;
            }
        }

        return m_physical_device != nullptr;
    }

    bool gpu_vulkan::init_physical_device_get_info_for_adapter(VkPhysicalDevice a_device, capabilities& a_out_caps,
        VkPhysicalDeviceFeatures2& a_out_features, vk_queue_families& a_out_queue_families)
    {
        capabilities caps = capabilities::k_none;
        
        VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        vkGetPhysicalDeviceProperties2(a_device, &props);

        VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        vkGetPhysicalDeviceFeatures2(a_device, &features);

        VkPhysicalDeviceMemoryProperties2 mem_props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2 };
        vkGetPhysicalDeviceMemoryProperties2(a_device, &mem_props);

        if(props.properties.apiVersion < VK_API_VERSION_1_2)
        {
            // Do not support APIs lower than 1_2 (TODO for now).
            return false;
        }

        if(props.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            reinterpret_cast<u32&>(caps) |= static_cast<u32>(capabilities::k_dedicated);
        }
        else if(props.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            // Do not support other types of GPU.
            return false;
        }

        if(props.properties.limits.maxComputeWorkGroupCount[0] > 0
            && props.properties.limits.maxComputeWorkGroupInvocations > 0
            && props.properties.limits.maxComputeWorkGroupSize[0] > 0)
        {
            reinterpret_cast<u32&>(caps) |= static_cast<u32>(capabilities::k_compute_pipeline);
        }

        if(features.features.tessellationShader)
        {
            reinterpret_cast<u32&>(caps) |= static_cast<u32>(capabilities::k_tesselation);
        }

        if(features.features.geometryShader)
        {
            reinterpret_cast<u32&>(caps) |= static_cast<u32>(capabilities::k_geometry_shading);
        }

        // Check supported queue types.
        u32 queue_family_num;
        vkGetPhysicalDeviceQueueFamilyProperties2(a_device, &queue_family_num, nullptr);
        data::array<VkQueueFamilyProperties2> queue_properties_array(queue_family_num);
        if(queue_family_num > 0)
        {
            for(u32 i = 0; i < queue_family_num; ++i)
            {
                queue_properties_array[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            }
            vkGetPhysicalDeviceQueueFamilyProperties2(a_device, &queue_family_num, queue_properties_array.data());

            data::set<u32> used_indices;
            for(u32 i = 0; i < static_cast<u32>(queue_type::k_enum_size); ++i)
            {
                queue_type type_to_set = static_cast<queue_type>(i);
                i32 last_index = -1;
                a_out_queue_families.family_indices[i] = last_index;    // Initialize to -1.

                VkQueueFlagBits flag_bits = VK_QUEUE_FLAG_BITS_MAX_ENUM;
                capabilities caps_to_set = capabilities::k_none;
                switch (type_to_set)
                {
                case queue_type::k_graphics:
                    flag_bits = VK_QUEUE_GRAPHICS_BIT;
                    break;
                case queue_type::k_compute:
                    flag_bits = VK_QUEUE_COMPUTE_BIT;
                    caps_to_set = capabilities::k_compute_queue;
                    break;
                case queue_type::k_transfer:
                    flag_bits = VK_QUEUE_TRANSFER_BIT;
                    caps_to_set = capabilities::k_transfer_queue;
                    break;
                default:
                    JE_fail("Unsupported queue type.");
                    break;
                }

                for(i32 j = 0; j < static_cast<i32>(queue_family_num); ++j)
                {
                    const VkQueueFamilyProperties2& queue_properties = queue_properties_array[j];
                    if(queue_properties.queueFamilyProperties.queueFlags & flag_bits)
                    {
                        const u32 index_to_use = static_cast<u32>(j);
                        if(used_indices.find(index_to_use) == used_indices.end())
                        {
                            used_indices.insert(index_to_use);
                            a_out_queue_families.family_indices[i] = index_to_use;
                            reinterpret_cast<u32&>(caps) |= static_cast<u32>(caps_to_set);
                            break;
                        }
                        else
                        {
                            last_index = index_to_use;
                        }
                    }
                }

                if(a_out_queue_families.family_indices[i] == -1)
                {
                    if(last_index >= 0)
                    {
                        a_out_queue_families.family_indices[i] = last_index;
                        reinterpret_cast<u32&>(caps) |= static_cast<u32>(caps_to_set);
                    }
                    // else this queue type is not supported - keep -1 value...
                }
            }

            if(a_out_queue_families.family_indices[static_cast<size>(queue_type::k_graphics)] < 0)
            {
                // Not applicable.
                return false;
            }
        }
        else
        {
            // No queue families? Fail...
            return false;
        }

        // TODO extract out more caps... For now let's hardcode them
        reinterpret_cast<u32&>(caps) |= static_cast<u32>(capabilities::k_transform_feedback);
        // Raytracing is per-logical device feature, will be queried later.

        a_out_caps = caps;
        a_out_features = features;
        return true;
    }

    bool gpu_vulkan::init_device(const VkPhysicalDeviceFeatures2& a_features, const vk_queue_families& a_queue_families)
    {
        // Set up queue indices for queue creation.
        data::set<u32> unique_queue_indices;
        for(size i = 0; i < static_cast<size>(queue_type::k_enum_size); ++i)
        {
            unique_queue_indices.insert(a_queue_families.family_indices[i]);
        }
        const u32 num_unique_queue_indices = static_cast<u32>(unique_queue_indices.size());
        data::array<VkDeviceQueueCreateInfo> queue_create_infos;
        queue_create_infos.reserve(num_unique_queue_indices);
        for(u32 index : unique_queue_indices)
        {
            static const f32 queue_priority = 1.0f; // TODO tweak these (very) later on.

            VkDeviceQueueCreateInfo queue_create_info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            queue_create_info.queueFamilyIndex = index;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;

            queue_create_infos.push_back(queue_create_info);
        }

        // Gather and verify properties.
        u32 property_num = 0;
        JE_vk_verify_bailout(vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &property_num, nullptr));
        if(property_num == 0)
        {
            // We need at least VK_KHR_swapchain.
            return false;
        }
        data::array<VkExtensionProperties> extension_properties(property_num);
        JE_vk_verify_bailout(vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &property_num, extension_properties.data()));

        static const data::static_array<const char*, 1> k_required_extensions = 
        {
            "VK_KHR_swapchain"
        };

        for(size i = 0; i < k_required_extensions.k_num_objects; ++i)
        {
            const char* required_extension = k_required_extensions[i];
            bool is_supported = false;
            for(size j = 0; j < property_num; ++j)
            {
                if(data::string::compare(required_extension, extension_properties[j].extensionName))
                {
                    is_supported = true;
                    break;
                }
            }

            if(is_supported == false)
            {
                JE_fail("Device extension [%s] is required but not supported!", required_extension);
                return false;
            }
        }

        // Create device.
        VkDeviceCreateInfo create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.queueCreateInfoCount = num_unique_queue_indices;
        create_info.pEnabledFeatures = &a_features.features;
        create_info.ppEnabledExtensionNames = k_required_extensions.get_data();
        create_info.enabledExtensionCount = k_required_extensions.k_num_objects;
        create_info.ppEnabledLayerNames = nullptr;
        create_info.enabledLayerCount = 0;  // Not necessary in newer Vulkan implementations.

        JE_vk_verify_bailout(vkCreateDevice(m_physical_device, &create_info, get_allocator(), &m_device));

        return true;
    }

    void gpu_vulkan::init_queues(const vk_queue_families& queue_families)
    {
        for(size i = 0; i < static_cast<size>(queue_type::k_enum_size); ++i)
        {
            vkGetDeviceQueue(m_device, queue_families.family_indices[i], 0, &m_queues[i]);
            JE_assert(m_queues[i] != VK_NULL_HANDLE);
        }
    }

    VkBool32 gpu_vulkan::debug_callback_static
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT a_message_severity,
        VkDebugUtilsMessageTypeFlagsEXT a_message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* a_callback_data,
        void* a_user_data
    )
    {
        reinterpret_cast<gpu_vulkan*>(a_user_data)->debug_callback(a_message_severity, a_message_type, a_callback_data->pMessage);
        return VK_FALSE;    // Do NOT abort vulkan call that caused this message. This will not be used for now.
    }

    void gpu_vulkan::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT a_message_severity,
        VkDebugUtilsMessageTypeFlagsEXT a_message_type, const char* a_message)
    {
        static const char* k_label_verbose = "VERBOSE";
        static const char* k_label_info = "INFO";
        static const char* k_label_warning = "WARNING";
        static const char* k_label_error = "ERROR";
        static const char* k_reason_general = "";
        static const char* k_reason_validation = " VALIDATION";
        static const char* k_reason_performance = " PERFORMANCE";

        // TODO configure severity via config or gpu_params.
        if(a_message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            const char* label = k_label_error;
            switch (a_message_severity)
            {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                label = k_label_verbose;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                label = k_label_info;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                label = k_label_warning;
                break;
            default:
                break;
            }

            const char* reason = k_reason_general;
            switch(a_message_type)
            {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                reason = k_reason_validation;
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                reason = k_reason_performance;
                break;
            default:
                break;
            }

            JE_print("VULKAN%s %s :\n%s", reason, label, a_message);
            if(a_message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            {
                JE_print_flush();
                JE_fail("A Vulkan error has occured.");
            }
        }
    }

}}