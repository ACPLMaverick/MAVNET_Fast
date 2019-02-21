#pragma once

#include "Resource.h"

namespace Rendering
{
	class Shader : public Resource
	{
	public:

		static const uint32_t MAX_SHADER_COUNT = 255;
		static const uint32_t KEY_PREFERRED_SIZE_BYTES = 1;

		JE_EnumBegin(Stage)

			Vertex,
			TesselationControl,
			TesselationEvaluation,
			Geometry,
			Fragment,
			Compute

		JE_EnumEnd()

		struct Key
		{
			uint8_t Uid : 8;

			bool operator==(const Key& other) const
			{
				JE_AssertStatic(sizeof(Key) <= KEY_PREFERRED_SIZE_BYTES);
				return Uid == other.Uid;
			}
		};

	public:

		static const char* StageToExtension[static_cast<uint8_t>(Stage::ENUM_SIZE)];


		Shader() { _type = ResourceCommon::Type::Shader; }
		~Shader() { }

		void Load(const std::string& shaderName, const ::Util::NullType* loadOpts = nullptr);
		void Cleanup();

		JE_Inline VkShaderModule GetModule(Stage stage) const { return _modules[static_cast<uint8_t>(Stage::ENUM_SIZE)]; }
		JE_Inline const Key* GetKey() const { return &_key; }
		JE_Inline bool IsLoaded() const { return _bIsLoaded; }
		void CreatePipelineShaderStageInfos(std::vector<VkPipelineShaderStageCreateInfo>* outShaderInfoArray) const;

	private:

		static bool LoadShader(const std::string& shaderName, Stage shaderType, std::vector<uint8_t>* outData);
		static VkShaderModule CreateShaderModule(const std::vector<uint8_t>* code);
		static VkShaderStageFlagBits ShaderStageToVkShaderStageFlag(Stage stage);

		VkShaderModule _modules[static_cast<uint8_t>(Stage::ENUM_SIZE)] = {};
		Key _key;
		bool _bIsLoaded = false;

	};
}