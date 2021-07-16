#include "shader_vulkan.h"
#include "device_vulkan.h"

namespace je { namespace draw { namespace gpu {

    shader_vulkan::shader_vulkan(const shader_params& a_params)
        : shader(a_params)
        , m_module(VK_NULL_HANDLE)
    {
    }

    shader_vulkan::~shader_vulkan()
    {
    }

    void shader_vulkan::shutdown(device& a_device)
    {
        if(m_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(JE_vk_device(a_device).get_device(), m_module, JE_vk_device(a_device).get_allocator());
            m_module = VK_NULL_HANDLE;
        }
    }

    bool shader_vulkan::init_module(device& a_device, const data::array<u8>& a_data)
    {
        JE_assert(a_data.data() != nullptr && a_data.size() > 0, "Invalid shader code buffer.");

        VkShaderModuleCreateInfo create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        create_info.pCode = reinterpret_cast<const u32*>(a_data.data());
        create_info.codeSize = a_data.size();

        JE_vk_verify_bailout(vkCreateShaderModule(JE_vk_device(a_device).get_device(), &create_info, JE_vk_device(a_device).get_allocator(), &m_module));
        return true;
    }

}}}