#include "global_vulkan.h"
#include "draw/gpu/params.h"

namespace je { namespace draw { namespace gpu {

    VkFormat texture_format_vk::to(texture_format a_format)
    {
        switch (a_format)
        {
        case texture_format::k_bgra8:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case texture_format::k_b10g11r11:
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case texture_format::k_bgra16:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case texture_format::k_rg16:
            return VK_FORMAT_R16G16_SFLOAT;
        case texture_format::k_r32:
            return VK_FORMAT_R32_SFLOAT;
        case texture_format::k_r16:
            return VK_FORMAT_R16_SFLOAT;
        case texture_format::k_r8:
            return VK_FORMAT_R8_UINT;
        case texture_format::k_d24s8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case texture_format::k_d32:
            return VK_FORMAT_D32_SFLOAT;
        case texture_format::k_d16:
            return VK_FORMAT_D16_UNORM;
        default:
            JE_todo();
            return VK_FORMAT_UNDEFINED;
        }
    }

    texture_format texture_format_vk::from(VkFormat a_format)
    {
        switch (a_format)
        {
        case VK_FORMAT_B8G8R8A8_SRGB:
            return texture_format::k_bgra8;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            return texture_format::k_b10g11r11;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return texture_format::k_bgra16;
        case VK_FORMAT_R16G16_SFLOAT:
            return texture_format::k_rg16;
        case VK_FORMAT_R32_SFLOAT:
            return texture_format::k_r32;
        case VK_FORMAT_R16_SFLOAT:
            return texture_format::k_r16;
        case VK_FORMAT_R8_UINT:
            return texture_format::k_r8;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return texture_format::k_d24s8;
        case VK_FORMAT_D32_SFLOAT:
            return texture_format::k_d32;
        case VK_FORMAT_D16_UNORM:
            return texture_format::k_d16;
        default:
            JE_todo();
            return texture_format::k_bgra8;
        }
    }

    VkShaderStageFlagBits shader_stage_vk::to(shader_stage a_stage)
    {
        switch (a_stage)
        {
        case shader_stage::k_vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case shader_stage::k_tess_ctrl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case shader_stage::k_tess_eval:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case shader_stage::k_geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case shader_stage::k_fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case shader_stage::k_compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        case shader_stage::k_rt_gen:
            return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        case shader_stage::k_rt_int:
            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        case shader_stage::k_rt_chit:
            return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        case shader_stage::k_rt_ahit:
            return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        case shader_stage::k_rt_miss:
            return VK_SHADER_STAGE_MISS_BIT_KHR;
        default:
            JE_todo();
            return VK_SHADER_STAGE_COMPUTE_BIT;
        }
    }

    shader_stage shader_stage_vk::from(VkShaderStageFlagBits a_stage)
    {
        switch (a_stage)
        {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return shader_stage::k_vertex;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return shader_stage::k_tess_ctrl;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return shader_stage::k_tess_eval;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return shader_stage::k_geometry;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return shader_stage::k_fragment;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return shader_stage::k_compute;
        case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
            return shader_stage::k_rt_gen;
        case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
            return shader_stage::k_rt_int;
        case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
            return shader_stage::k_rt_chit;
        case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
            return shader_stage::k_rt_ahit;
        case VK_SHADER_STAGE_MISS_BIT_KHR:
            return shader_stage::k_rt_miss;
        default:
            JE_todo();
            return shader_stage::k_compute;
        }
    }

}}}