#pragma once

#include "RenderPassCommon.h"
#include "Rendering/pipeline/Pipeline.h"
#include "Rendering/resource/Attachment.h"

namespace Rendering
{
	class RenderPass
	{
	public:

		static const size_t MAX_ATTACHMENTS = 7;
		static const size_t MAX_SUBPASSES = 15;
		static const uint8_t INVALID_SUBPASS_IDX = 0x1F;

		// TODO: Make this not vulkan-specific. Somehow.
		struct Dependency
		{
			uint32_t StageMaskSource;
			uint32_t StageMaskDest;
			uint32_t AccessMaskSource;
			uint32_t AccessMaskDest;
			uint8_t SubpassIndexSource : 5;
			uint8_t SubpassIndexDest : 5;
			uint8_t Flags : 3;

			Dependency();
		};

		struct Subpass
		{
			Dependency MyDependency;
			uint8_t ColorAttachmentIndices[MAX_ATTACHMENTS];
			uint8_t DepthAttachmentIndex;
			Pipeline::Type BindPoint;
			uint8_t NumColorAttachmentIndices;
		};

		struct Info
		{
			Attachment::AttachDesc ColorAttachments[MAX_ATTACHMENTS];
			Attachment::AttachDesc DepthStencilAttachments[MAX_ATTACHMENTS];

			Subpass Subpasses[MAX_SUBPASSES];
			uint8_t NumColorAttachments : 4;
			uint8_t NumDepthAttachments : 4;
			uint8_t NumSubpasses;

			bool operator==(const Info& other) const
			{
				if (NumColorAttachments != other.NumColorAttachments)
				{
					return false;
				}

				if (NumDepthAttachments != other.NumDepthAttachments)
				{
					return false;
				}

				if (NumSubpasses != other.NumSubpasses)
				{
					return false;
				}

				if (NumColorAttachments > 0)
				{
					if (ColorAttachments[0].Format != other.ColorAttachments[0].Format)
					{
						return false;
					}
				}

				return std::memcmp(this, &other, sizeof(Info)) == 0;
			}
		};

	public:

		RenderPass() { }
		~RenderPass();

		void Initialize(const Info* info);
		void Cleanup();
		void Reinitialize();

		JE_Inline const Info* GetInfo() const { return &_info; }
		JE_Inline VkRenderPass GetVkRenderPass() const { return _renderPass; }

	private:

		Info _info;

		VkRenderPass _renderPass = VK_NULL_HANDLE;
	};
}

namespace std
{
	template<> struct hash<::Rendering::RenderPass::Info>
	{
		size_t operator()(const ::Rendering::RenderPass::Info& key) const
		{
			JE_AssertStatic(sizeof(::Rendering::RenderPass::Info) % sizeof(size_t) == 0);
			size_t finalRet = 0;
			const size_t hashNum = sizeof(::Rendering::RenderPass::Info) / sizeof(size_t);
			const size_t hashes[hashNum] = {};

			const size_t* dataPtr = reinterpret_cast<const size_t*>(&key);
			for (size_t i = 0; i < hashNum; ++i, ++dataPtr)
			{
				finalRet = (finalRet ^ std::hash<size_t>{}(*dataPtr) << i) >> 1;
			}

			return finalRet;
		}
	};
}