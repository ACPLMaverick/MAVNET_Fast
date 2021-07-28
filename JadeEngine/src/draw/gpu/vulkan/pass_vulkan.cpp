#include "pass_vulkan.h"
#include "device_vulkan.h"

namespace je { namespace draw { namespace gpu {

    pass_vulkan::pass_vulkan(const pass_params& a_params)
        : pass(a_params)
        , m_render_pass(VK_NULL_HANDLE)
    {

    }

    pass_vulkan::~pass_vulkan()
    {
        JE_assert(m_render_pass == VK_NULL_HANDLE);
    }

    bool pass_vulkan::init(device& a_device, const pass_params& a_params)
    {
        device_vulkan& device = JE_vk_device(a_device);

        JE_assert_bailout(m_params.m_render_target_infos.get_size() > 0, false, "Empty render target info for render pass creation.");
        JE_assert_bailout(m_params.m_ops.get_size() > 0, false, "Empty operations for render pass creation.");

        data::static_stack<VkAttachmentDescription2, pass_params::render_target_infos::k_num_objects> attachment_descriptions;
        for(size i = 0; i < m_params.m_render_target_infos.get_size(); ++i)
        {
            const render_target_info& info = m_params.m_render_target_infos[i];
            
            VkAttachmentDescription2 desc{ VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2 };
            desc.format = texture_format_vk::to(info.m_format);
            desc.samples = VK_SAMPLE_COUNT_1_BIT;   // multisampling not supported atm.
            desc.loadOp = info.m_is_cleared ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            const bool is_depth = is_texture_format_depth(info.m_format);
            const bool is_stencil = is_texture_format_depth_stencil(info.m_format);
            if(is_stencil)
            {
                desc.stencilLoadOp = desc.loadOp;
                desc.stencilStoreOp = desc.storeOp;
            }
            else
            {
                desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            }
            desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // TODO ?
            desc.finalLayout = info.m_is_present_surface ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : is_depth ? VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
            attachment_descriptions.push(desc);
        }
        JE_assert(attachment_descriptions.get_size() > 0, "Empty attachment descriptions.");
        
        data::static_stack<VkSubpassDescription2, pass_params::operations::k_num_objects> subpass_descriptions;
        for(size i = 0; i < m_params.m_ops.get_size(); ++i)
        {
            const pass_operation& op = m_params.m_ops[i];

            JE_assert_bailout(op.m_output_render_target_indices.get_size() > 0, false, "Pass operation thad doesn't write to any RT is not allowed.");

            VkSubpassDescription2 desc{ VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 };
            data::static_stack<VkAttachmentReference2, pass_operation::inputs::k_num_objects> input_attachments;
            data::static_stack<VkAttachmentReference2, pass_operation::outputs::k_num_objects> output_attachments;
            VkAttachmentReference2 depth_attachment{};
            
            for(size op_idx = 0; op_idx < op.m_input_render_target_indices.get_size(); ++op_idx)
            {
                const u32 rt_idx = op.m_input_render_target_indices[op_idx];
                const render_target_info& rt_info = m_params.m_render_target_infos[rt_idx];

                VkAttachmentReference2 attachment_reference{ VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 };
                attachment_reference.attachment = rt_idx;
                attachment_reference.layout = get_layout_for_rt_info(rt_info, true);

                input_attachments.push(attachment_reference);
            }

            for(size op_idx = 0; op_idx < op.m_output_render_target_indices.get_size(); ++op_idx)
            {
                const u32 rt_idx = op.m_output_render_target_indices[op_idx];
                const render_target_info& rt_info = m_params.m_render_target_infos[rt_idx];

                VkAttachmentReference2 attachment_reference{ VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 };
                attachment_reference.attachment = rt_idx;
                attachment_reference.layout = get_layout_for_rt_info(rt_info, false);

                if(is_texture_format_depth(rt_info.m_format))
                {
                    JE_assert(depth_attachment.layout == VK_IMAGE_LAYOUT_UNDEFINED, "There can be only one depth output attachment.");
                    depth_attachment = attachment_reference;
                }
                else
                {
                    output_attachments.push(attachment_reference);
                }
            }

            desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            desc.inputAttachmentCount = input_attachments.get_size();
            desc.pInputAttachments = input_attachments.get_data();
            desc.colorAttachmentCount = output_attachments.get_size();
            desc.pColorAttachments = output_attachments.get_data();
            if(depth_attachment.layout != VK_IMAGE_LAYOUT_UNDEFINED)
            {
                desc.pDepthStencilAttachment = &depth_attachment;
            }
            else
            {
                desc.pDepthStencilAttachment = nullptr;
            }
            // Not supported.
            desc.pResolveAttachments = nullptr;

            subpass_descriptions.push(desc);
        }

        JE_assert_bailout(subpass_descriptions.get_size() > 0, false, "No subpasses in created render pass.");
        VkRenderPassCreateInfo2 rp_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 };
        rp_info.attachmentCount = attachment_descriptions.get_size();
        rp_info.pAttachments = attachment_descriptions.get_data();
        rp_info.subpassCount = subpass_descriptions.get_size();
        rp_info.pSubpasses = subpass_descriptions.get_data();

        if(subpass_descriptions.get_size() > 1)
        {
            // If > 1 subpasses, we need to specify dependences between them.
            JE_todo();
        }
        else
        {
            rp_info.dependencyCount = 0;
            rp_info.pDependencies = nullptr;
        }

        JE_vk_verify_bailout(vkCreateRenderPass2(device.get_device(), &rp_info, device.get_allocator(), &m_render_pass));

        return true;
    }

    void pass_vulkan::shutdown(device& a_device)
    {
        device_vulkan& device = JE_vk_device(a_device);
        if(m_render_pass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(device.get_device(), m_render_pass, device.get_allocator());
            m_render_pass = VK_NULL_HANDLE;
        }
    }

    VkImageLayout pass_vulkan::get_layout_for_rt_info(const render_target_info& a_info, bool a_is_input)
    {
        if(a_is_input)
        {
            if(is_texture_format_depth(a_info.m_format))
            {
                return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            }
            else
            {
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
        }
        else
        {
            if(is_texture_format_depth(a_info.m_format))
            {
                return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            }
            else
            {
                if(a_info.m_is_present_surface)
                {
                    return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }
                else
                {
                    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
            }
        }
    }

}}}