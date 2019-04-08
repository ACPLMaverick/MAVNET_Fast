#pragma once

#include "Texture.h"

namespace Rendering
{
	class Attachment : public Texture
	{
	public:

		JE_EnumBegin(UsageMode)
			Color
			, ColorPresentable
			, Transferable
			, DepthStencil
		JE_EnumEnd()

		struct AttachDesc
		{
			uint32_t Format = {};
			RenderState::MultisamplingMode MyMultisamplingMode : 2;
			UsageMode Usage : 2;	// Is deduced from Format and entry point.
			bool bClearOnLoad : 1;
			bool bStore : 1;
			bool bUseStencil : 1;	// Is deduced from Format.
		};

	public:

		static void CreateAttachmentsFromSwapchain(std::vector<Attachment*>& outAttachments);

		virtual void Create(const CreateOptions* createOptions) override;
		virtual void Resize(const ResizeInfo* resizeInfo) override;

		const AttachDesc& GetAttachDesc() const { return _attachDesc; }
		bool IsSwapChainImage() const { return _attachDesc.Usage == UsageMode::ColorPresentable; }
		bool IsDepthStencil() const { return _attachDesc.Usage == UsageMode::DepthStencil; }

	protected:

		virtual void ClearWithFixedValue(const glm::vec4& clearValuesNormalized) override;
		virtual VkImageAspectFlagBits ObtainImageAspect() override;
		virtual VkImageLayout ObtainDestLayout() override;

		void CreateFromSwapChainImage(const CreateOptions* createOptions, VkImage swapChainImage);
		void BuildAttachDesc(const CreateOptions* createOptions);


		static const uint32_t BAD_SWAP_CHAIN_INDEX = ~0;

		AttachDesc _attachDesc;
	};
}