#include "RenderState.h"

namespace Rendering
{
	void RenderState::Initialize(const Info * info)
	{
		_info = *info;
		CreateKey();
	}

	void RenderState::Cleanup()
	{
		// Do nothing special for now.
		_info = {};
		_key = {};
	}

	void RenderState::CreateInputAssembly(VkPipelineInputAssemblyStateCreateInfo * outIAState) const
	{
		outIAState->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		outIAState->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // hardcoded
		outIAState->primitiveRestartEnable = VK_FALSE; // hardcoded
	}

	void RenderState::CreateViewportState(VkViewport* outViewport, VkRect2D* outScissor, VkPipelineViewportStateCreateInfo* outViewportState) const
	{
		CreateViewport(outViewport);
		CreateScissor(outScissor);

		outViewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		outViewportState->viewportCount = 1;
		outViewportState->pViewports = outViewport;
		outViewportState->scissorCount = 1;
		outViewportState->pScissors = outScissor;
	}

	void RenderState::CreateRasterizationState(VkPipelineRasterizationStateCreateInfo * outRasterizationState) const
	{
		outRasterizationState->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		outRasterizationState->depthClampEnable = _info.bRasterizerDepthClamp;	// This is useful in some special cases like shadow maps. 		  State->
		outRasterizationState->rasterizerDiscardEnable = !_info.bRasterizerEnabled; // If is set to VK_TRUE, then geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
		outRasterizationState->polygonMode = ConvertToVkPolygonMode(_info.DrawMode);
		outRasterizationState->lineWidth = 1.0f; // hardcoded
		outRasterizationState->cullMode = VK_CULL_MODE_BACK_BIT; // hardcoded
		outRasterizationState->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // hardcoded
		outRasterizationState->depthBiasEnable = VK_FALSE; // hardcoded
		outRasterizationState->depthBiasConstantFactor = 0.0f; // hardcoded
		outRasterizationState->depthBiasClamp = 0.0f; // hardcoded
		outRasterizationState->depthBiasSlopeFactor = 0.0f; // hardcoded
	}

	void RenderState::CreateMultisampleState(VkSampleMask* outSampleMask, VkPipelineMultisampleStateCreateInfo * outMultisampleState) const
	{
		*(outSampleMask) = _info.SampleMask;

		outMultisampleState->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		outMultisampleState->sampleShadingEnable = _info.bSampleShading;
		outMultisampleState->rasterizationSamples = ConvertToVkSampleCount(_info.SampleCount);
		outMultisampleState->minSampleShading = 1.0f; // hardcoded !!
		outMultisampleState->pSampleMask = *outSampleMask != 0xFFFFFFFF ? outSampleMask : nullptr;
		outMultisampleState->alphaToCoverageEnable = _info.bAlphaToCoverage;
		outMultisampleState->alphaToOneEnable = _info.bAlphaToOne;
	}

	void RenderState::CreateDepthStencilState(VkPipelineDepthStencilStateCreateInfo * outDepthStencilState) const
	{
		outDepthStencilState->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		outDepthStencilState->depthTestEnable = _info.bDepthTestEnabled;
		outDepthStencilState->depthWriteEnable = _info.bDepthWriteEnabled;
		outDepthStencilState->depthCompareOp = ConvertToVkCompareOp(_info.DepthCompareOperation);
		outDepthStencilState->depthBoundsTestEnable = VK_FALSE; // hardcoded
		outDepthStencilState->minDepthBounds = 0.0f; // hardcoded
		outDepthStencilState->maxDepthBounds = 1.0f; // hardcoded
		outDepthStencilState->stencilTestEnable = _info.bStencilTestEnabled;

		if (_info.bStencilTestEnabled)
		{
			outDepthStencilState->front.failOp = ConvertToVkStencilOp(_info.StencilOpFail);
			outDepthStencilState->front.passOp = ConvertToVkStencilOp(_info.StencilOpPass);
			outDepthStencilState->front.compareOp = ConvertToVkCompareOp(_info.StencilOpCompare);
			outDepthStencilState->front.compareMask = _info.StencilCompareMask;
			outDepthStencilState->front.writeMask = _info.StencilWriteMask;
			outDepthStencilState->back = outDepthStencilState->front;
		}
		else
		{
			outDepthStencilState->front = {};
			outDepthStencilState->back = {};
		}
	}

	void RenderState::CreateColorBlendAttachments(std::vector<VkPipelineColorBlendAttachmentState>* outAttachmentStates, VkPipelineColorBlendStateCreateInfo * outColorBlendAttachments) const
	{
		for (uint32_t i = 0; i < _info.FramebufferCount; ++i)
		{
			VkPipelineColorBlendAttachmentState attachmentState = {};
			CreateColorBlendAttachment(i, &attachmentState);
			outAttachmentStates->push_back(attachmentState);
		}

		outColorBlendAttachments->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		outColorBlendAttachments->logicOpEnable = VK_FALSE;	// If set to true, this overwrites attachment state settings. hardcoded
		outColorBlendAttachments->logicOp = VK_LOGIC_OP_COPY;		// ???????????? hardcoded
		outColorBlendAttachments->attachmentCount = _info.FramebufferCount;
		outColorBlendAttachments->pAttachments = outAttachmentStates->data();
		outColorBlendAttachments->blendConstants[0] = 0.0f;
		outColorBlendAttachments->blendConstants[1] = 0.0f;
		outColorBlendAttachments->blendConstants[2] = 0.0f;
		outColorBlendAttachments->blendConstants[3] = 0.0f;
	}

	uint8_t RenderState::PackResolution16to8(uint16_t value)
	{
		static const uint32_t upperValue = 4096;	// This is the value 256 corresponds to. So the input has to be lesser than that.
		JE_Assert(value < upperValue);
		static const uint16_t divisor = upperValue / (static_cast<uint8_t>(-1) + 1);
		return static_cast<uint8_t>(value / divisor);
	}

	uint16_t RenderState::UnpackResolution8to16(uint8_t value)
	{
		// TODO: This code won't work for higher resolutions (i.e. won't decode 1080).
		// Need to use some look-up table and store index here :(.
		JE_Assert(false);
		static const uint32_t upperValue = 4096;	// This is the value 256 corresponds to.
		static const uint16_t multiplier = upperValue / (static_cast<uint8_t>(-1) + 1);
		return static_cast<uint16_t>(value) * multiplier;
	}

	VkPolygonMode RenderState::ConvertToVkPolygonMode(PolygonDrawMode value)
	{
		switch (value)
		{
		case Rendering::RenderState::PolygonDrawMode::Solid:
			return VK_POLYGON_MODE_FILL;
		case Rendering::RenderState::PolygonDrawMode::Wireframe:
			return VK_POLYGON_MODE_LINE;
		default:
			JE_Assert(false);
			return VK_POLYGON_MODE_MAX_ENUM;
		}
	}


	VkSampleCountFlagBits RenderState::ConvertToVkSampleCount(MultisamplingMode value)
	{
		switch (value)
		{
		case MultisamplingMode::None:
			return VK_SAMPLE_COUNT_1_BIT;
		case MultisamplingMode::Samples_4:
			return VK_SAMPLE_COUNT_4_BIT;
		case MultisamplingMode::Samples_8:
			return VK_SAMPLE_COUNT_8_BIT;
		case MultisamplingMode::Samples_16:
			return VK_SAMPLE_COUNT_16_BIT;
		default:
			JE_Assert(false);
			return VK_SAMPLE_COUNT_1_BIT;
			break;
		}
	}

	VkCompareOp RenderState::ConvertToVkCompareOp(CompareOperation value)
	{
		switch (value)
		{
		case Rendering::RenderState::CompareOperation::Less:
			return VK_COMPARE_OP_LESS;
		case Rendering::RenderState::CompareOperation::Equal:
			return VK_COMPARE_OP_EQUAL;
		case Rendering::RenderState::CompareOperation::LessEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case Rendering::RenderState::CompareOperation::Greater:
			return VK_COMPARE_OP_GREATER;
		case Rendering::RenderState::CompareOperation::GreaterEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Rendering::RenderState::CompareOperation::NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;
		default:
			JE_Assert(false);
			return VK_COMPARE_OP_MAX_ENUM;
		}
	}

	VkStencilOp RenderState::ConvertToVkStencilOp(StencilOperation value)
	{
		switch (value)
		{
		case Rendering::RenderState::StencilOperation::Keep:
			return VK_STENCIL_OP_KEEP;
		case Rendering::RenderState::StencilOperation::Zero:
			return VK_STENCIL_OP_ZERO;
		case Rendering::RenderState::StencilOperation::Replace:
			return VK_STENCIL_OP_REPLACE;
		case Rendering::RenderState::StencilOperation::Invert:
			return VK_STENCIL_OP_INVERT;
		default:
			JE_Assert(false);
			return VK_STENCIL_OP_MAX_ENUM;
		}
	}

	VkBlendFactor RenderState::ConvertToVkBlendFactor(ColorBlend::BlendFactor value)
	{
		switch (value)
		{
		case Rendering::RenderState::ColorBlend::BlendFactor::Zero:
			return VK_BLEND_FACTOR_ZERO;
		case Rendering::RenderState::ColorBlend::BlendFactor::Disabled:
		case Rendering::RenderState::ColorBlend::BlendFactor::One:
			return VK_BLEND_FACTOR_ONE;
		case Rendering::RenderState::ColorBlend::BlendFactor::SrcColor:
			return VK_BLEND_FACTOR_SRC_COLOR;
		case Rendering::RenderState::ColorBlend::BlendFactor::OneMinusSrcColor:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case Rendering::RenderState::ColorBlend::BlendFactor::DstColor:
			return VK_BLEND_FACTOR_DST_COLOR;
		case Rendering::RenderState::ColorBlend::BlendFactor::OneMinusDstColor:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case Rendering::RenderState::ColorBlend::BlendFactor::SrcAlpha:
			return VK_BLEND_FACTOR_SRC_ALPHA;
		case Rendering::RenderState::ColorBlend::BlendFactor::OneMinusSrcAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case Rendering::RenderState::ColorBlend::BlendFactor::DstAlpha:
			return VK_BLEND_FACTOR_DST_ALPHA;
		case Rendering::RenderState::ColorBlend::BlendFactor::OneMinusDstAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		default:
			JE_Assert(false);
			return VK_BLEND_FACTOR_MAX_ENUM;
		}
	}

	VkBlendOp RenderState::ConvertToVkBlendOp(ColorBlend::BlendOperation value)
	{
		switch (value)
		{
		case Rendering::RenderState::ColorBlend::BlendOperation::Add:
			return VK_BLEND_OP_ADD;
		case Rendering::RenderState::ColorBlend::BlendOperation::Subtract:
			return VK_BLEND_OP_SUBTRACT;
		case Rendering::RenderState::ColorBlend::BlendOperation::Min:
			return VK_BLEND_OP_MIN;
		case Rendering::RenderState::ColorBlend::BlendOperation::Max:
			return VK_BLEND_OP_MAX;
		default:
			JE_Assert(false);
			return VK_BLEND_OP_MAX_ENUM;
		}
	}

	void RenderState::CreateKey()
	{
		_key.DrawMode = static_cast<uint8_t>(_info.DrawMode);
		_key.ViewportPositionWidth = PackResolution16to8(_info.ViewportWidth);
		_key.ViewportPositionHeight = PackResolution16to8(_info.ViewportHeight);
		_key.ScissorPositionX = PackResolution16to8(_info.ScissorPositionX);
		_key.ScissorPositionY = PackResolution16to8(_info.ScissorPositionY);
		_key.ScissorPositionWidth = PackResolution16to8(_info.ScissorWidth);
		_key.ScissorPositionHeight = PackResolution16to8(_info.ScissorHeight);
		_key.bRasterizerDepthClamp = _info.bRasterizerDepthClamp;
		_key.bRasterizerEnabled = _info.bRasterizerEnabled;
		_key.SampleMask = _info.SampleMask;
		_key.SampleCount = static_cast<uint8_t>(_info.SampleCount);
		_key.bSampleShading = _info.bSampleShading;
		_key.bAlphaToCoverage = _info.bAlphaToCoverage;
		_key.bAlphaToOne = _info.bAlphaToOne;
		_key.DepthCompareOperation = static_cast<uint8_t>(_info.DepthCompareOperation);
		_key.bDepthDestEnabled = _info.bDepthTestEnabled;
		_key.bDepthWriteEnabled = _info.bDepthWriteEnabled;
		_key.StencilOpFail = static_cast<uint8_t>(_info.StencilOpFail);
		_key.StencilOpPass = static_cast<uint8_t>(_info.StencilOpPass);
		_key.StencilCompareOperation = static_cast<uint8_t>(_info.StencilOpCompare);
		_key.StencilCompareMask = _info.StencilCompareMask;
		_key.StencilWriteMask = _info.StencilWriteMask;
		_key.bStencilTestEnabled = static_cast<uint8_t>(_info.bStencilTestEnabled);
		_key.FramebufferCountMinusOne = static_cast<uint8_t>(_info.FramebufferCount - 1);

		_key.SrcBlendFactorFb01 = static_cast<uint8_t>(_info.ColorBlends[0].SrcBlendFactor);
		_key.DstBlendFactorFb01 = static_cast<uint8_t>(_info.ColorBlends[0].DstBlendFactor);
		_key.BlendOperationFb01 = static_cast<uint8_t>(_info.ColorBlends[0].ColorOperation);

		_key.SrcBlendFactorFb02 = static_cast<uint8_t>(_info.ColorBlends[1].SrcBlendFactor);
		_key.DstBlendFactorFb02 = static_cast<uint8_t>(_info.ColorBlends[1].DstBlendFactor);
		_key.BlendOperationFb02 = static_cast<uint8_t>(_info.ColorBlends[1].ColorOperation);

		_key.SrcBlendFactorFb03 = static_cast<uint8_t>(_info.ColorBlends[2].SrcBlendFactor);
		_key.DstBlendFactorFb03 = static_cast<uint8_t>(_info.ColorBlends[2].DstBlendFactor);
		_key.BlendOperationFb03 = static_cast<uint8_t>(_info.ColorBlends[2].ColorOperation);

		_key.SrcBlendFactorFb04 = static_cast<uint8_t>(_info.ColorBlends[3].SrcBlendFactor);
		_key.DstBlendFactorFb04 = static_cast<uint8_t>(_info.ColorBlends[3].DstBlendFactor);
		_key.BlendOperationFb04 = static_cast<uint8_t>(_info.ColorBlends[3].ColorOperation);
	}

	void RenderState::CreateViewport(VkViewport * outViewport) const
	{
		outViewport->x = 0.0f;  // hardcoded
		outViewport->y = 0.0f;  // hardcoded
		outViewport->width = static_cast<float>(_info.ViewportWidth);
		outViewport->height = static_cast<float>(_info.ViewportHeight);
		outViewport->minDepth = 0.0f;  // hardcoded
		outViewport->maxDepth = 1.0f;  // hardcoded
	}

	void RenderState::CreateScissor(VkRect2D * outScissor) const
	{
		outScissor->offset.x = _info.ScissorPositionX;
		outScissor->offset.y = _info.ScissorPositionY;
		outScissor->extent.width = _info.ScissorWidth;
		outScissor->extent.height = _info.ScissorHeight;
	}

	void RenderState::CreateColorBlendAttachment(uint32_t framebufferIndex, VkPipelineColorBlendAttachmentState * outAttachmentState) const
	{
		outAttachmentState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		if (_info.ColorBlends[framebufferIndex].DstBlendFactor == ColorBlend::BlendFactor::Disabled ||
			_info.ColorBlends[framebufferIndex].SrcBlendFactor == ColorBlend::BlendFactor::Disabled)
		{
			outAttachmentState->blendEnable = VK_FALSE;
		}
		else
		{
			outAttachmentState->blendEnable = VK_TRUE;

			outAttachmentState->srcColorBlendFactor = ConvertToVkBlendFactor(_info.ColorBlends[framebufferIndex].SrcBlendFactor);
			outAttachmentState->dstColorBlendFactor = ConvertToVkBlendFactor(_info.ColorBlends[framebufferIndex].DstBlendFactor);
			outAttachmentState->colorBlendOp = ConvertToVkBlendOp(_info.ColorBlends[framebufferIndex].ColorOperation);
			outAttachmentState->srcAlphaBlendFactor = outAttachmentState->srcColorBlendFactor;
			outAttachmentState->dstAlphaBlendFactor = outAttachmentState->dstColorBlendFactor;
			outAttachmentState->alphaBlendOp = outAttachmentState->colorBlendOp;
		}
	}
}