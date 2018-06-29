#include "Pipeline.h"


namespace Rendering
{
	void Pipeline::Initialize(const RenderState::Info * renderStateInfo, const Shader * shader)
	{
		JE_Assert(renderStateInfo != nullptr && shader != nullptr);

		_shader = shader;

		_renderState.Initialize(renderStateInfo);

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
	}

	void Pipeline::Cleanup()
	{
		_shader = nullptr;
		_renderState.Cleanup();
	}
}