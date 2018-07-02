#pragma once

namespace Rendering
{
	class RenderState
	{
	public:

		static const uint32_t PACKED_STATE_PREFERRED_SIZE_BYTES = 28;
		static const uint32_t MAX_COLOR_FRAMEBUFFERS_ATTACHED = 4;


		JE_EnumBegin(PolygonDrawMode)
			  Solid
			, Wireframe
		JE_EnumEnd()

		JE_EnumBegin(MultisamplingMode)
			  None
			, Samples_4
			, Samples_8
			, Samples_16
		JE_EnumEnd()

		JE_EnumBegin(CompareOperation)
			  Less
			, Equal
			, LessEqual
			, Greater
			, GreaterEqual
			, NotEqual
		JE_EnumEnd()

		JE_EnumBegin(StencilOperation) // Other ops are not supported so this can fit into 2 bits
			  Keep
			, Zero
			, Replace
			, Invert
		JE_EnumEnd()

		struct ColorBlend
		{
			JE_EnumBegin(BlendFactor)	// Other ops are not supported.
				  Disabled
				, Zero
				, One
				, SrcColor
				, OneMinusSrcColor
				, DstColor
				, OneMinusDstColor
				, SrcAlpha
				, OneMinusSrcAlpha
				, DstAlpha
				, OneMinusDstAlpha
			JE_EnumEnd()

			JE_EnumBegin(BlendOperation)	// Reverse subtract and other weird stuff is not supported.
				  Add
				, Subtract
				, Min
				, Max
			JE_EnumEnd()

			// TODO: Would like to have color write mask here very much.
			// Both the same operations for alpha and color!
			BlendFactor SrcBlendFactor = {};
			BlendFactor DstBlendFactor = {};
			BlendOperation ColorOperation = {};
		};
		
		struct Info
		{
			// Topology & rasterization
			PolygonDrawMode DrawMode = {};

			// Viewport
			// Moving viewports not supported.
			uint16_t ViewportWidth = 0;
			uint16_t ViewportHeight = 0;
			// TODO: Will probably have to support this in the future. Think of something better to optimize.

			// Scissor
			uint16_t ScissorPositionX = 0;
			uint16_t ScissorPositionY = 0;
			uint16_t ScissorWidth = 0;
			uint16_t ScissorHeight = 0;

			// Rasterization
			bool bRasterizerDepthClamp = false;
			bool bRasterizerEnabled = false;

			// Multisampling
			uint32_t SampleMask = 0;
			MultisamplingMode SampleCount = {};
			// TODO: Want to minSampleShading here, or disable this completely.
			bool bSampleShading = false;
			bool bAlphaToCoverage = false;
			bool bAlphaToOne = false;

			// DepthStencil (stencil only for front faces)
			CompareOperation DepthCompareOperation = {};
			bool bDepthTestEnabled = false;
			bool bDepthWriteEnabled = false;

			//? Simplify this by removing stencil operations on fail and pass (default keep/discard)?
			CompareOperation StencilOpCompare = {};
			StencilOperation StencilOpFail = {};
			StencilOperation StencilOpPass = {};
			uint32_t StencilCompareMask = 0;
			uint32_t StencilWriteMask = 0;
			bool bStencilTestEnabled = false;

			// ColorBlends for four supported attached framebuffers.
			ColorBlend ColorBlends[MAX_COLOR_FRAMEBUFFERS_ATTACHED] = {};
			uint32_t FramebufferCount = 0;

			// Fuck DynamicStates, we don't use it for now.
		};

		// This has to be as small as possible, preferably 16 bytes (128 bits)
		// Now it's 28 bytes. Will have to live with that for now.
#pragma pack(push, 1)
		struct Key
		{
			uint32_t SampleMask	: 32;
			uint32_t StencilCompareMask : 32;
			uint32_t StencilWriteMask : 32;
			uint8_t ViewportPositionWidth : 8;
			uint8_t ViewportPositionHeight : 8;
			uint8_t ScissorPositionX : 8;
			uint8_t ScissorPositionY : 8;
			uint8_t ScissorPositionWidth : 8;
			uint8_t ScissorPositionHeight : 8;

			uint8_t SrcBlendFactorFb01 : 4;
			uint8_t DstBlendFactorFb01 : 4;
			uint8_t SrcBlendFactorFb02 : 4;
			uint8_t DstBlendFactorFb02 : 4;
			uint8_t SrcBlendFactorFb03 : 4;
			uint8_t DstBlendFactorFb03 : 4;
			uint8_t SrcBlendFactorFb04 : 4;
			uint8_t DstBlendFactorFb04 : 4;

			uint8_t DepthCompareOperation : 3;
			uint8_t StencilCompareOperation : 3;
			uint8_t SampleCount : 2;
			uint8_t StencilOpFail : 2;
			uint8_t StencilOpPass : 2;
			uint8_t FramebufferCountMinusOne : 2;

			uint8_t BlendOperationFb01 : 2;
			uint8_t BlendOperationFb02 : 2;
			uint8_t BlendOperationFb03 : 2;
			uint8_t BlendOperationFb04 : 2;

			uint8_t DrawMode : 1;
			uint8_t bRasterizerDepthClamp : 1;
			uint8_t bRasterizerEnabled : 1;
			uint8_t bSampleShading : 1;
			uint8_t bAlphaToCoverage : 1;
			uint8_t bAlphaToOne : 1;
			uint8_t bDepthDestEnabled : 1;
			uint8_t bDepthWriteEnabled : 1;
			uint8_t bStencilTestEnabled : 1;

			bool operator==(const Key& other) const
			{
				JE_AssertStatic(sizeof(Key) <= PACKED_STATE_PREFERRED_SIZE_BYTES);
				const uint32_t arrayNum = PACKED_STATE_PREFERRED_SIZE_BYTES / sizeof(uint64_t);
				const uint64_t* arrayMy = reinterpret_cast<const uint64_t*>(this);
				const uint64_t* arrayHim = reinterpret_cast<const uint64_t*>(&other);
				for (size_t i = 0; i < arrayNum; ++i)
				{
					if (arrayMy[i] != arrayHim[i])
						return false;
				}

				return true;
			}
		};
#pragma pack(pop)


		// ++ Helper conversion functions.
		
		static VkPolygonMode ConvertToVkPolygonMode(PolygonDrawMode value);
		static VkSampleCountFlagBits ConvertToVkSampleCount(MultisamplingMode value);
		static VkCompareOp ConvertToVkCompareOp(CompareOperation value);
		static VkStencilOp ConvertToVkStencilOp(StencilOperation value);
		static VkBlendFactor ConvertToVkBlendFactor(ColorBlend::BlendFactor value);
		static VkBlendOp ConvertToVkBlendOp(ColorBlend::BlendOperation value);
		
		// -- Helper conversion functions.


		RenderState() { }
		~RenderState() { }

		void Initialize(const Info* info);
		void Cleanup();

		JE_Inline const Info* GetInfo() const { return &_info; }
		JE_Inline const Key* GetPackedState() const { return &_key; }
		
		void CreateInputAssembly(VkPipelineInputAssemblyStateCreateInfo* outIAState) const;
		void CreateViewportState(VkViewport* outViewport, VkRect2D* outScissor, VkPipelineViewportStateCreateInfo* outViewportState) const;
		void CreateRasterizationState(VkPipelineRasterizationStateCreateInfo* outRasterizationState) const;
		void CreateMultisampleState(VkSampleMask* outSampleMask, VkPipelineMultisampleStateCreateInfo* outMultisampleState) const;
		void CreateDepthStencilState(VkPipelineDepthStencilStateCreateInfo* outDepthStencilState) const;
		void CreateColorBlendAttachments(std::vector<VkPipelineColorBlendAttachmentState>* outAttachmentStates, VkPipelineColorBlendStateCreateInfo* outColorBlendAttachments) const;

	private:

		Info _info;
		// My packed state used for comparison.
		Key _key;

		static uint8_t PackResolution16to8(uint16_t value);
		static uint16_t UnpackResolution8to16(uint8_t value);

		void CreateKey();

		void CreateViewport(VkViewport* outViewport) const;
		void CreateScissor(VkRect2D* outScissor) const;
		void CreateColorBlendAttachment(uint32_t framebufferIndex, VkPipelineColorBlendAttachmentState* outAttachmentState) const;

	};
}