#include "pso_vulkan.h"
#include "device_vulkan.h"
#include "shader_vulkan.h"
#include "pass_vulkan.h"

namespace je { namespace draw { namespace gpu {

    bool pso_vulkan::update_for_new_size(math::screen_size a_new_size)
    {
        // const math::screen_size wanted_viewport_size = compute_screen_size(a_new_size, m_state.m_viewport_perc_offset);
        // const math::screen_size wanted_scissor_size = compute_screen_size(a_new_size, m_state.m_scissor_perc_offset);
        JE_todo();
        return false;
    }

    pso_vulkan::pso_vulkan(const pso_params& a_params)
        : pso(a_params)
        , m_layout(VK_NULL_HANDLE)
        , m_pipeline(VK_NULL_HANDLE)
    {
    }

    pso_vulkan::~pso_vulkan()
    {
        
    }

    bool pso_vulkan::init(device& a_device, const pso_params& a_params)
    {
        // TODO
        VkPipelineVertexInputStateCreateInfo vertex_input_info{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexAttributeDescriptions = nullptr;
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.pVertexBindingDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo input_assembly_info{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_info.primitiveRestartEnable = false;
        input_assembly_info.flags = 0;  // Not supported by API.

        const math::screen_size wanted_viewport_size = compute_screen_size(a_params.m_source_screen_size, m_state.m_static.m_viewport_perc_offset);
        const math::screen_size wanted_scissor_size = compute_screen_size(a_params.m_source_screen_size, m_state.m_static.m_scissor_perc_offset);
        VkViewport viewport{};
        viewport.x = static_cast<float>(m_state.m_static.m_viewport_perc_offset.z);
        viewport.y = static_cast<float>(m_state.m_static.m_viewport_perc_offset.w);
        viewport.width = static_cast<float>(wanted_viewport_size.x);
        viewport.height = static_cast<float>(wanted_viewport_size.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{};
        scissor.offset.x = m_state.m_static.m_scissor_perc_offset.z;
        scissor.offset.y = m_state.m_static.m_scissor_perc_offset.w;
        scissor.extent.width = wanted_scissor_size.x;
        scissor.extent.height = wanted_scissor_size.y;

        VkPipelineViewportStateCreateInfo viewport_info{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewport_info.viewportCount = 1;
        viewport_info.pViewports = &viewport;
        viewport_info.scissorCount = 1;
        viewport_info.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasteriser_info{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasteriser_info.depthClampEnable = m_state.m_static.m_shadow_depth_bias_and_clamp;
        rasteriser_info.rasterizerDiscardEnable = VK_FALSE;
        rasteriser_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasteriser_info.cullMode = m_state.m_static.m_cull_backface ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
        rasteriser_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasteriser_info.depthBiasEnable = m_state.m_static.m_shadow_depth_bias_and_clamp;
        rasteriser_info.depthBiasConstantFactor = m_state.m_static.m_shadow_depth_bias_and_clamp ? 0.0f : 0.0f; // TODO
        rasteriser_info.depthBiasClamp = m_state.m_static.m_shadow_depth_bias_and_clamp ? 0.0f : 0.0f;   // TODO
        rasteriser_info.depthBiasSlopeFactor = m_state.m_static.m_shadow_depth_bias_and_clamp ? 0.0f : 0.0f; // TODO

        VkPipelineMultisampleStateCreateInfo multisample_info{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisample_info.sampleShadingEnable = m_state.has_valid_multisampling();
        if(multisample_info.sampleShadingEnable)
        {
            JE_todo(); // TODO
        }
        else
        {
            multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisample_info.minSampleShading = 1.0f;
            multisample_info.pSampleMask = nullptr;
            multisample_info.alphaToCoverageEnable = VK_FALSE;
            multisample_info.alphaToOneEnable = VK_FALSE;
        }

        VkPipelineDepthStencilStateCreateInfo ds_info{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        ds_info.depthTestEnable = m_state.m_static.m_depth_test_mode != depth_test_mode::k_none;
        ds_info.depthWriteEnable = m_state.m_static.m_depth_write;
        switch(m_state.m_static.m_depth_test_mode)
        {
        case depth_test_mode::k_equal:
            ds_info.depthCompareOp = VK_COMPARE_OP_EQUAL;
            break;
        case depth_test_mode::k_less_equal:
            ds_info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            break;
        case depth_test_mode::k_none:
            ds_info.depthCompareOp = VK_COMPARE_OP_ALWAYS;
        default:
            JE_fail("Not implemented.");
            break;
        }
        ds_info.depthBoundsTestEnable = VK_FALSE;   // TODO consider if this is needed.
        ds_info.stencilTestEnable = m_state.m_static.m_stencil_mode != stencil_mode::k_none;
        switch (m_state.m_static.m_stencil_mode)
        {
        case stencil_mode::k_write:
            ds_info.front.failOp = VK_STENCIL_OP_REPLACE;
            ds_info.front.passOp = VK_STENCIL_OP_REPLACE;
            ds_info.front.depthFailOp = VK_STENCIL_OP_KEEP;
            ds_info.front.compareOp = VK_COMPARE_OP_ALWAYS;
            ds_info.front.compareMask = 0xFF;
            ds_info.front.writeMask = m_state.m_static.m_stencil_value;
            ds_info.front.reference = 0xFF; // ???
            break;
        case stencil_mode::k_test:
            ds_info.front.failOp = VK_STENCIL_OP_KEEP;
            ds_info.front.passOp = VK_STENCIL_OP_REPLACE;
            ds_info.front.depthFailOp = VK_STENCIL_OP_KEEP;
            ds_info.front.compareOp = VK_COMPARE_OP_EQUAL;
            ds_info.front.compareMask = m_state.m_static.m_stencil_value;
            ds_info.front.writeMask = 0;
            ds_info.front.reference = 0xFF; // ???
            break;
        case stencil_mode::k_none:
            break;
        default:
            JE_fail("Not implemented.");
            break;
        }
        ds_info.back = ds_info.front;
        ds_info.minDepthBounds = 0.0f;
        ds_info.maxDepthBounds = 1.0f;

        VkPipelineColorBlendAttachmentState blend_attachment{}; // TODO One for all attachments. May need to configure in the future.
        blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blend_attachment.blendEnable = m_state.m_static.m_blend_mode != blend_mode::k_overwrite;
        switch(m_state.m_static.m_blend_mode)
        {
        case blend_mode::k_alpha_based:
            blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        case blend_mode::k_additive:
            JE_fail("Please check if this is correct."); // TODO
            blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        case blend_mode::k_overwrite:
            blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
            break;
        default:
            JE_fail("Not implemented.");
            break;
        }

        VkPipelineColorBlendStateCreateInfo blend_info{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        blend_info.logicOpEnable = VK_FALSE;
        blend_info.attachmentCount = 1; // TODO More in the future??
        blend_info.pAttachments = &blend_attachment;

        static const data::static_array<VkDynamicState, 2> dynamic_states = 
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_info{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamic_info.dynamicStateCount = dynamic_states.k_num_objects;
        dynamic_info.pDynamicStates = dynamic_states.get_data();

        // Layouts TODO
        VkPipelineLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        layout_info.setLayoutCount = 0;
        layout_info.pSetLayouts = nullptr;
        layout_info.pushConstantRangeCount = 0;
        layout_info.pPushConstantRanges = nullptr;

        JE_vk_verify_bailout(vkCreatePipelineLayout(JE_vk_device(a_device).get_device(), &layout_info, JE_vk_device(a_device).get_allocator(), &m_layout));

        // Shaders.
        data::static_stack<VkPipelineShaderStageCreateInfo, static_cast<size>(shader_stage::k_enum_size)> shader_stages;
        for(size i = 0; i < static_cast<size>(shader_stage::k_enum_size); ++i)
        {
            shader_vulkan* shader_module = reinterpret_cast<shader_vulkan*>(m_state.m_dynamic.m_shaders[i]);
            shader_stage stage = static_cast<shader_stage>(i);

            if(shader_module != nullptr)
            {
                VkPipelineShaderStageCreateInfo* stage_info = shader_stages.push_uninitialized();
                stage_info->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                stage_info->flags = 0;
                stage_info->stage = shader_stage_vk::to(stage);
                stage_info->module = shader_module->get_module();
                stage_info->pName = "main"; // TODO?
                stage_info->pSpecializationInfo = nullptr;
            }
        }

        JE_verify_bailout(shader_stages.get_size() > 0, false, "No shaders have been assigned to this PSO.");

        // Push constants.


        VkGraphicsPipelineCreateInfo pipeline_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipeline_info.stageCount = shader_stages.get_size();
        pipeline_info.pStages = shader_stages.get_data();
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly_info;
        pipeline_info.pViewportState = &viewport_info;
        pipeline_info.pRasterizationState = &rasteriser_info;
        pipeline_info.pMultisampleState = &multisample_info;
        pipeline_info.pDepthStencilState = &ds_info;
        pipeline_info.pColorBlendState = &blend_info;
        pipeline_info.pDynamicState = &dynamic_info;
        pipeline_info.layout = m_layout;
        pipeline_info.renderPass = reinterpret_cast<const pass_vulkan&>(a_params.m_pass).get_render_pass();
        pipeline_info.subpass = a_params.m_subpass_idx;
        pipeline_info.basePipelineHandle = a_params.m_base_pso != nullptr ? reinterpret_cast<pso_vulkan*>(a_params.m_base_pso)->get_pipeline() : VK_NULL_HANDLE;
        pipeline_info.basePipelineIndex = -1; // Unused.

        // TODO Pipeline cache.
        JE_vk_verify_bailout(vkCreateGraphicsPipelines(JE_vk_device(a_device).get_device(), nullptr, 1, &pipeline_info, JE_vk_device(a_device).get_allocator(), &m_pipeline));
        
        return true;
    }

    void pso_vulkan::shutdown(device& a_device)
    {
        if(m_pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(JE_vk_device(a_device).get_device(), m_pipeline, JE_vk_device(a_device).get_allocator());
            m_pipeline = VK_NULL_HANDLE;
        }
        if(m_layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(JE_vk_device(a_device).get_device(), m_layout, JE_vk_device(a_device).get_allocator());
            m_layout = VK_NULL_HANDLE;
        }
    }

}}}