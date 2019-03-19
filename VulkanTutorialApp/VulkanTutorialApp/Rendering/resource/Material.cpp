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
		, _descriptorSet(nullptr)
		, _uboPerObject(nullptr)
		, _uboGlobal(nullptr)
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

		// This too.
		UniformBuffer::Options options;
		options.DataSize = sizeof(UboCommon::StaticMeshCommon);
		_uboPerObject = new UniformBuffer();
		_uboPerObject->Initialize(&options);

		options.DataSize = sizeof(UboCommon::SceneGlobal);
		_uboGlobal = new UniformBuffer();
		_uboGlobal->Initialize(&options);

		// This as well.
		::Rendering::Texture::LoadOptions texOptions;
		std::string texName = "texture.jpg";
		_textures.push_back(JE_GetApp()->GetResourceManager()->CacheTextures.Get(texName, &texOptions));

		// TODO: Why the fuck bindings are global and not per-stage?
		DescriptorSet::Info info;
		info.LayInfo.Bindings[0] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Vertex,
			0,
			1,
			ResourceCommon::Type::UniformBuffer
		);

		info.LayInfo.Bindings[1] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Fragment,
			1,
			1,
			ResourceCommon::Type::UniformBuffer
		);
		info.LayInfo.Bindings[2] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Fragment,
			2,
			1,
			ResourceCommon::Type::Texture2D
		);
		info.Resources[0][0] = _uboPerObject;
		info.Resources[1][0] = _uboGlobal;
		info.Resources[2][0] = _textures[0];
		_descriptorSet = JE_GetRenderer()->GetManagerDescriptor()->Get(&info);


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
		pipelineInfo.DescriptorLayoutData = _descriptorSet->GetAssociatedLayout();
		pipelineInfo.MyVertexDeclaration = &_vertexDeclaration;
		pipelineInfo.MyType = Pipeline::Type::Graphics;
		pipelineInfo.MyPass = RenderPassCommon::Id::Tutorial;

		Pipeline::CreateKey(&pipelineInfo, &_pipelineKey);

		_pipeline = JE_GetRenderer()->GetManagerPipeline()->Get(&_pipelineKey, &pipelineInfo);
	}

	void Material::Update()
	{
		UpdateUboPerObject();	// TODO: Move this to transform class.
		UpdateUboGlobal();
	}

	void Material::Cleanup()
	{
		_textures.clear();

		_vertexDeclaration = VertexDeclaration();

		if (_uboPerObject != nullptr)	// TODO: Do not remove, instead make manager do this.
		{
			_uboPerObject->Cleanup();
			delete _uboPerObject;
			_uboPerObject = nullptr;
		}
		if (_uboGlobal != nullptr)	// TODO: Do not cleanup, instead make manager do this.
		{
			_uboGlobal->Cleanup();
			delete _uboGlobal;
			_uboGlobal = nullptr;
		}

		_pipeline = nullptr;

		// TODO: Implement proper non test code.
	}

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

		mv_translation = mv_translation * rotation * scale;

		const glm::mat4& viewProj = *JE_GetRenderer()->GetCamera()->GetViewProj();
		const glm::mat4& view = *JE_GetRenderer()->GetCamera()->GetView();

		matrices.MVP = viewProj * mv_translation;
		matrices.MV = view * mv_translation;
		matrices.MVInverseTranspose = glm::transpose(glm::inverse(matrices.MV));

		_uboPerObject->UpdateWithData(reinterpret_cast<uint8_t*>(&matrices), sizeof(matrices));
	}

	void Material::UpdateUboGlobal()
	{
		const Fog& fog = *JE_GetRenderer()->GetFog();
		const LightDirectional& lightDirectional = *JE_GetRenderer()->GetLightDirectional();

		Rendering::UboCommon::SceneGlobal pco;
		pco.FogColor = *fog.GetColor();
		pco.FogDistNear = fog.GetStartDistance();
		pco.FogDistFar = fog.GetEndDistance();
		pco.LightColor = *lightDirectional.GetColor();
		pco.InvLightDirectionV = -*lightDirectional.GetDirectionV();

		_uboGlobal->UpdateWithData(reinterpret_cast<uint8_t*>(&pco), sizeof(pco));
	}

}