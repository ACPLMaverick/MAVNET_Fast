#pragma once

namespace Rendering
{
	class Pipeline;
	class Texture;

	class Material
	{
	public:

		class VertexDeclaration
		{
		public:

			enum Type
			{
				POSITION,
				COLOR,
				NORMAL,
				UV,
				TANGENT,
				BINORMAL,
				WEIGHT,
				INDEX
			};

		public:

			static size_t GetComponentSize();

			VertexDeclaration();
			VertexDeclaration(const VertexDeclaration& copy);
			~VertexDeclaration();

			void Init(const std::vector<Type>* components);

			bool IsHavingComponent() const;
			const std::vector<Type>* GetComponents() const;
			void GetComponentSizes(std::vector<size_t>* sizeVector) const;

			void GetBindingDescription(VkVertexInputBindingDescription& outDescription);
			void GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& outDescriptions);

			bool operator==(const VertexDeclaration& other) const;

		private:

			static const size_t _typeToSizeBytes[];

			std::vector<Type> _components;
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

	protected:

		VertexDeclaration _vertexDeclaration;

		std::vector<Texture*> _textures;

		Pipeline* _pipeline;
	};
}