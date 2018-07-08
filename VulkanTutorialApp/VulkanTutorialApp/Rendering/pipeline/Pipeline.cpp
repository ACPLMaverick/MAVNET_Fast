#include "Pipeline.h"

#include "Core/HelloTriangle.h"
#include "ManagerPipeline.h"
#include "Rendering/renderPass/ManagerRenderPass.h"

namespace Rendering
{
	void Pipeline::Initialize
	(
		const Info* initData
	)
	{
		JE_Assert
		(
			initData != nullptr
			&& initData->RenderStateInfo != nullptr
			&& initData->MyShader != nullptr
			&& initData->DescriptorLayoutData != nullptr
			&& initData->MyVertexDeclaration != nullptr
		);

		_associatedShader = initData->MyShader;
		_associatedDescriptorLayoutData = initData->DescriptorLayoutData;
		_associatedVertexDeclaration = initData->MyVertexDeclaration;
		RenderPassKey key = static_cast<RenderPassKey>(initData->MyPass);
		_associatedRenderPass = JE_GetRenderer()->GetManagerRenderPass()->Get(&key);
		_type = initData->MyType;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		_associatedShader->CreatePipelineShaderStageInfos(&shaderStages);

		// Pipeline layout creation.

		VkDescriptorSetLayout layouts[] = { _associatedDescriptorLayoutData->Layout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;	// hardcoded. Only one set layout is supported for now.
		pipelineLayoutInfo.pSetLayouts = layouts;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;	// hardcoded. Push constant range are not supported for now.

		JE_AssertThrowVkResult(vkCreatePipelineLayout(JE_GetRenderer()->GetDevice(), &pipelineLayoutInfo, JE_GetRenderer()->GetAllocatorPtr(), &_associatedPipelineLayout));


		// Pipeline Herself Majesty First Of Her Name Creation.

		if (initData->MyType == Type::Graphics)
		{
			_renderState.Initialize(initData->RenderStateInfo);

			std::vector<VkVertexInputBindingDescription> bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			_associatedVertexDeclaration->GetBindingDescriptions(&bindingDescriptions);
			_associatedVertexDeclaration->GetAttributeDescriptions(&attributeDescriptions);

			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			VkViewport viewport = {};
			VkRect2D scissor = {};
			VkPipelineViewportStateCreateInfo viewportState = {};
			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			VkSampleMask multisamplingMask;
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			VkPipelineDepthStencilStateCreateInfo depthStencil = {};

			std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
			VkPipelineColorBlendStateCreateInfo colorBlending = {};

			_renderState.CreateInputAssembly(&inputAssembly);
			_renderState.CreateViewportState(&viewport, &scissor, &viewportState);
			_renderState.CreateRasterizationState(&rasterizer);
			_renderState.CreateMultisampleState(&multisamplingMask, &multisampling);
			_renderState.CreateDepthStencilState(&depthStencil);
			_renderState.CreateColorBlendAttachments(&colorBlendAttachments, &colorBlending);

			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

			pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineInfo.pStages = shaderStages.data();

			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = nullptr;

			pipelineInfo.layout = _associatedPipelineLayout;
			pipelineInfo.renderPass = _associatedRenderPass->GetVkRenderPass();
			pipelineInfo.subpass = 0;

			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex = -1;
			//pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT; // You can use this to derive from existing pipeline which is faster than creating one from scratch.

			JE_AssertThrowVkResult(vkCreateGraphicsPipelines(JE_GetRenderer()->GetDevice(), nullptr, 1, &pipelineInfo, JE_GetRenderer()->GetAllocatorPtr(), &_pipeline));
		}
		else if (initData->MyType == Type::Compute)
		{
			VkComputePipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;

			JE_Assert(shaderStages.size() == 1);
			pipelineInfo.stage = shaderStages[0];
			pipelineInfo.layout = _associatedPipelineLayout;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex = -1;

			JE_AssertThrowVkResult(vkCreateComputePipelines(JE_GetRenderer()->GetDevice(), nullptr, 1, &pipelineInfo, JE_GetRenderer()->GetAllocatorPtr(), &_pipeline));
		}
		else
		{
			JE_Assert(false);
		}
	}

	void Pipeline::Cleanup()
	{
		JE_Assert(_associatedShader != nullptr);
		JE_Assert(_associatedDescriptorLayoutData != nullptr);
		_associatedShader = nullptr;
		_associatedDescriptorLayoutData = nullptr;
		_associatedRenderPass = nullptr;
		_renderState.Cleanup();
	}

	void Pipeline::CreateKey(const Info* info, Key* outKey)
	{
		// TODO: Make it good.
	}
}