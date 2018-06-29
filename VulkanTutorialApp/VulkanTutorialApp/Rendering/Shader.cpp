#include "Shader.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{
	const char* Shader::StageToExtension[]
	{
		".vert",
		".tesc",
		".tese",
		".geom",
		".frag",
		".comp"
	};


	void Shader::Load(const char * shaderName)
	{
		_key = {};
		std::vector<uint8_t> shaderData;
		uint64_t keys[static_cast<size_t>(Stage::ENUM_SIZE)] = {};
		bool hasAnyShader = false;
		for (size_t i = 0; i < static_cast<size_t>(Stage::ENUM_SIZE); ++i)
		{
			// Clear the module for safety.
			_modules[i] = VK_NULL_HANDLE;

			Stage stage = static_cast<Stage>(i);
			shaderData.clear();
			if (!LoadShader(shaderName, stage, &shaderData))
			{
				continue;
			}

			hasAnyShader = true;

			// Create module from data.
			_modules[i] = CreateShaderModule(&shaderData);

			// Perform reflection on this module
			// TODO: Reflection.

			// Assign an index to this module and check it.
			keys[i] = JE_GetRenderer()->GetManagerUid()->ObtainUidForShaderModule(stage);
		}

		JE_Assert(hasAnyShader);

		// Create key.
		_key.UidStageVertex = keys[static_cast<size_t>(Stage::Vertex)];
		_key.UidStageTesselationControl = keys[static_cast<size_t>(Stage::TesselationControl)];
		_key.UidStageTesselationEvaluation = keys[static_cast<size_t>(Stage::TesselationEvaluation)];
		_key.UidStageGeometry = keys[static_cast<size_t>(Stage::Geometry)];
		_key.UidStageFragment = keys[static_cast<size_t>(Stage::Fragment)];
		_key.UidStageCompute = keys[static_cast<size_t>(Stage::Compute)];

		_bIsLoaded = true;
	}

	void Shader::Cleanup()
	{
		// Unload & destroy modules.
		for (size_t i = 0; i < static_cast<size_t>(Stage::ENUM_SIZE); ++i)
		{
			if(_modules[i] == VK_NULL_HANDLE)
				continue;

			vkDestroyShaderModule(JE_GetRenderer()->GetDevice(), _modules[i], JE_GetRenderer()->GetAllocatorPtr());
			_modules[i] = VK_NULL_HANDLE;
		}

		_bIsLoaded = false;
	}

	void Shader::CreatePipelineShaderStageInfos(std::vector<VkPipelineShaderStageCreateInfo>* outShaderInfoArray) const
	{
		outShaderInfoArray->clear();
		for (size_t i = 0; i < static_cast<size_t>(Stage::ENUM_SIZE); ++i)
		{
			if (_modules[i] == VK_NULL_HANDLE)
				continue;

			VkPipelineShaderStageCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			createInfo.stage = ShaderStageToVkShaderStageFlag(static_cast<Stage>(i));
			createInfo.module = _modules[i];
			createInfo.pName = "main";	// It's possible to combine multiple fragment shaders into a single shader module and use different entry points to differentiate between their behaviors. 
			createInfo.pSpecializationInfo = nullptr; //  It allows you to specify values for shader constants. You can use a single shader module where its behavior can be configured at pipeline creation by specifying different values for the constants used in it. 

			outShaderInfoArray->push_back(createInfo);
		}
	}

	bool Shader::LoadShader(const char* shaderName, Stage shaderType, std::vector<uint8_t>* outData)
	{
		return ::Core::HelloTriangle::LoadFile(::Core::HelloTriangle::RESOURCE_PATH + "Shaders\\Binary\\" + shaderName + StageToExtension[static_cast<uint8_t>(shaderType)] + ".spv", *outData);
	}

	VkShaderModule Shader::CreateShaderModule(const std::vector<uint8_t>* code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code->size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code->data());

		VkShaderModule shaderModule;
		JE_AssertThrowVkResult(vkCreateShaderModule(JE_GetRenderer()->GetDevice(), &createInfo, JE_GetRenderer()->GetAllocatorPtr(), &shaderModule));
		return shaderModule;
	}

	VkShaderStageFlagBits Shader::ShaderStageToVkShaderStageFlag(Stage stage)
	{
		switch (stage)
		{
		case Stage::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case Stage::TesselationControl:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case Stage::TesselationEvaluation:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case Stage::Geometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case Stage::Fragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case Stage::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		default:
			JE_Assert(false);
			return VK_SHADER_STAGE_ALL_GRAPHICS;
		}
	}

}