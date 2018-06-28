#pragma once

namespace Rendering
{
	class Pipeline;
	class Texture;
	class DescriptorSet;
	class UniformBuffer;

	class Material
	{
	public:

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

			VertexDeclaration(const VertexDeclaration& copy);

		private:

			std::vector<ComponentType> _components;
		};

		struct UBOPerScene
		{
			JE_AlignAs(16) glm::vec3 LightColor;
			JE_AlignAs(16) glm::vec3 LightDirectionV;
			float FogDepthNear;
			glm::vec3 FogColor;
			float FogDepthFar;
		};

		struct UBOPerObject
		{
			glm::mat4 MVP;
			glm::mat4 MV;
			glm::mat4 MVInverseTranspose;

			UBOPerObject()
				: MVP(glm::mat4(1.0f))
				, MV(glm::mat4(1.0f))
				, MVInverseTranspose(glm::mat4(1.0f))
			{
			}
		};

	public:
		Material();
		~Material();

		void Initialize();
		void Update();
		void Cleanup();

		JE_Inline const VertexDeclaration* GetVertexDeclaration() const { return &_vertexDeclaration; }
		JE_Inline const DescriptorSet* GetDescriptorSet() const { return _descriptorSet; }
		JE_Inline const UniformBuffer* GetUboPerObject() const { return _uboPerObject; }
		JE_Inline const UniformBuffer* GetUboPerMaterial() const { return _uboPerMaterial; }
		JE_Inline const Pipeline* GetPipeline() const { return _pipeline; }

	protected:

		VertexDeclaration _vertexDeclaration;

		std::vector<Texture*> _textures;

		DescriptorSet* _descriptorSet;
		
		UniformBuffer* _uboPerObject;
		UniformBuffer* _uboPerMaterial;

		Pipeline* _pipeline;


		void UpdateUboPerObject();
		void UpdateUboPerMaterial();
	};
}