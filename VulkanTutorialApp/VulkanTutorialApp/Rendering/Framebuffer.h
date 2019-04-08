#pragma once

#include "Rendering/renderPass/RenderPass.h"
#include "Rendering/renderStep/RenderStepCommon.h"

namespace Rendering
{
	class Attachment;

	class Framebuffer
	{
	public:

		static const uint32_t MAX_COLOR_ATTACHMENTS = RenderPass::MAX_ATTACHMENTS;
		static const uint32_t MAX_DEPTH_ATTACHMENTS = RenderPass::MAX_ATTACHMENTS;

		struct Info
		{
			Attachment* ColorAttachments[MAX_COLOR_ATTACHMENTS] = {};
			Attachment* DepthAttachments[MAX_DEPTH_ATTACHMENTS] = {};
			RenderStepCommon::Id AssociatedRenderStep = std::numeric_limits<RenderStepCommon::Id>::max();
			uint8_t NumColorAttachments = 0;
			uint8_t NumDepthAttachments = 0;
		};

	public:

		Framebuffer();
		~Framebuffer();

		void Initialize(const Info* info);
		void Cleanup();

		const Info* GetInfo() const { return &_info; }
		uint32_t GetWidth() const { return _width; }
		uint32_t GetHeight() const { return _height; }
		bool HasDepthStencil() const { return _info.NumDepthAttachments != 0; }
		VkFramebuffer GetVkFramebuffer() const { return _framebuffer; }

	private:

		void CreateVkFramebuffer();

		Info _info;
		VkFramebuffer _framebuffer;
		uint32_t _width;
		uint32_t _height;
	};
}