#pragma once

namespace Rendering
{
	class VertexDeclaration
	{
	public:

		JE_EnumBegin(ComponentType)

			Position,
			Color,
			Normal,
			Uv,
			Tangent,
			Binormal,
			Weight,
			Index

		JE_EnumEnd()

	public:

		static JE_Inline uint32_t GetComponentSize(ComponentType type) { return _typeToSizeBytes[static_cast<uint8_t>(type)]; }
		static JE_Inline VkFormat GetComponentFormat(ComponentType type) { return _typeToVkFormat[static_cast<uint8_t>(type)]; }

		VertexDeclaration();
		~VertexDeclaration();

		void Initialize(const std::vector<ComponentType>* components);

		bool IsHavingComponent(ComponentType type) const;
		JE_Inline const std::vector<ComponentType>* GetComponents() const { return &_components; }
		void GetComponentSizes(std::vector<uint32_t>* sizeVector) const;
		uint32_t GetComponentTotalSize() const;

		void GetBindingDescriptions(std::vector<VkVertexInputBindingDescription>* outDescriptions) const;
		void GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>* outDescriptions) const;

		bool operator==(const VertexDeclaration& other) const;

	private:

		static constexpr const uint32_t _typeToSizeBytes[] =
		{
			12,		// vec3
			16,		// vec4
			12,		// vec3
			8,		// vec2
			12,		// vec3
			12,		// vec3
			16,		// vec4
			8		// ushort4
		};

		static constexpr const VkFormat _typeToVkFormat[] =
		{
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32_UINT		// For 8 bytes, will have to unpack it probably.
		};

	private:

		std::vector<ComponentType> _components;
	};
}