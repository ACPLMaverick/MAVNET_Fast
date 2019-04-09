#include "Material.h"

#include "Core/HelloTriangle.h"
#include "Texture.h"
#include "Rendering/resource/buffer/UniformBuffer.h"

#include "Rendering/descriptor/ManagerDescriptor.h"
#include "Rendering/descriptor/DescriptorSet.h"

namespace Rendering
{
	Material::Material()
		: Resource()
	{
		_type = ResourceCommon::Type::Material;
	}


	Material::~Material()
	{
	}

	void Material::Load(const std::string& name, const ::Util::NullType* loadOptions /*= nullptr*/)
	{
		// TODO: Implement proper non test code.

		// This is created via Shader Reflection.
		std::vector<VertexDeclaration::ComponentType> components;
		components.push_back(VertexDeclaration::ComponentType::Position);
		components.push_back(VertexDeclaration::ComponentType::Color);
		components.push_back(VertexDeclaration::ComponentType::Normal);
		components.push_back(VertexDeclaration::ComponentType::Uv);
		_vertexDeclaration.Initialize(&components);

		// This as well.
		::Rendering::Texture::LoadOptions texOptions;
		std::string texName = "texture.jpg";
		_textures.push_back(JE_GetApp()->GetResourceManager()->CacheTextures.Get(texName, &texOptions));

		// This too.
		// TODO: Why the fuck bindings are global and not per-stage?
		DescriptorCommon::LayoutInfo layoutInfo;
		layoutInfo.Bindings[0] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Vertex,
			0,
			1,
			ResourceCommon::Type::UniformBuffer
		);

		layoutInfo.Bindings[1] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Fragment,
			1,
			1,
			ResourceCommon::Type::UniformBuffer
		);
		layoutInfo.Bindings[2] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Fragment,
			2,
			1,
			ResourceCommon::Type::Texture2D
		);
		_descriptorLayout = JE_GetRenderer()->GetManagerDescriptor()->GetDescriptorLayout(&layoutInfo);


		Pipeline::Info pipelineInfo = {};

		// Loaded from file too.
		pipelineInfo.RenderStateInfo.bAlphaToCoverage = false;
		pipelineInfo.RenderStateInfo.bAlphaToOne = false;
		pipelineInfo.RenderStateInfo.bDepthTestEnabled = true;
		pipelineInfo.RenderStateInfo.bDepthWriteEnabled = true;
		pipelineInfo.RenderStateInfo.bRasterizerDepthClamp = false;
		pipelineInfo.RenderStateInfo.bRasterizerEnabled = true;
		pipelineInfo.RenderStateInfo.bSampleShading = false;
		pipelineInfo.RenderStateInfo.bStencilTestEnabled = false;

		pipelineInfo.RenderStateInfo.ColorBlends[0].SrcBlendFactor = RenderState::ColorBlend::BlendFactor::Disabled;
		pipelineInfo.RenderStateInfo.ColorBlends[0].DstBlendFactor = RenderState::ColorBlend::BlendFactor::Disabled;

		pipelineInfo.RenderStateInfo.DepthCompareOperation = RenderState::CompareOperation::Less;
		pipelineInfo.RenderStateInfo.DrawMode = RenderState::PolygonDrawMode::Solid;
		pipelineInfo.RenderStateInfo.FramebufferCount = 1; // TODO: This should be defined by render pass.
		pipelineInfo.RenderStateInfo.SampleCount = RenderState::MultisamplingMode::None;
		pipelineInfo.RenderStateInfo.SampleMask = 0xFFFFFFFF;
		pipelineInfo.RenderStateInfo.ScissorWidth = JE_GetRenderer()->GetSwapChainExtent().width;
		pipelineInfo.RenderStateInfo.ScissorHeight = JE_GetRenderer()->GetSwapChainExtent().height;
		pipelineInfo.RenderStateInfo.ViewportWidth = JE_GetRenderer()->GetSwapChainExtent().width;
		pipelineInfo.RenderStateInfo.ViewportHeight = JE_GetRenderer()->GetSwapChainExtent().height;


		pipelineInfo.MyShader = JE_GetApp()->GetResourceManager()->CacheShaders.Get("TutorialShader");
		pipelineInfo.DescriptorLayoutData = &_descriptorLayout;
		pipelineInfo.MyVertexDeclaration = &_vertexDeclaration;
		pipelineInfo.MyType = Pipeline::Type::Graphics;
		pipelineInfo.MyRenderStep = (RenderStepCommon::Id)RenderStepCommon::FixedId::Tutorial;	// TODO: Pipelines should be created for every render step that this material can be drawn in.

		Pipeline::CreateKey(&pipelineInfo, &_pipelineKey);

		_pipeline = JE_GetRenderer()->GetManagerPipeline()->Get(&_pipelineKey, &pipelineInfo);
	}

	void Material::Update()
	{
	}

	void Material::Cleanup()
	{
		_textures.clear();

		_vertexDeclaration = VertexDeclaration();

		_pipeline = nullptr;
	}

	/*
	void Material::UpdateUboPerObject()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//float time = 0.0f;

		UboCommon::StaticMeshCommon matrices;

		glm::mat4 mv_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	}
	*/
}