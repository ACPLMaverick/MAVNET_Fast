#pragma once

#include "Resource.h"
#include "VertexDeclaration.h"

namespace Rendering
{
	class Pipeline;
	class Texture;
	class DescriptorSet;
	class UniformBuffer;

	class Material : public Resource
	{
	public:

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
		JE_Inline const UniformBuffer* GetUboPerMaterial() const { return _uboGlobal; }
		JE_Inline const Pipeline* GetPipeline() const { return _pipeline; }

	protected:

		VertexDeclaration _vertexDeclaration;

		std::vector<Texture*> _textures;

		DescriptorSet* _descriptorSet;
		
		UniformBuffer* _uboPerObject;
		UniformBuffer* _uboGlobal;

		Pipeline* _pipeline;


		void UpdateUboPerObject();
		void UpdateUboGlobal();
	};
}