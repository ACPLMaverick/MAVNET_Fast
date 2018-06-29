#pragma once

#include "Rendering/Shader.h"

namespace Core
{
	class ManagerUid
	{
	public:

		ManagerUid() { }
		~ManagerUid() { }

		void Initialize();
		void Cleanup();

		Uid ObtainUidForShaderModule(Rendering::Shader::Stage stage);
		Uid ObtainUidForMisc();

		void FreeUidForShaderModule(Rendering::Shader::Stage stage, Uid uid);
		void FreeUidForMisc(Uid uid);

	private:

		Uid _uidsShaderModule[static_cast<uint8_t>(Rendering::Shader::Stage::ENUM_SIZE)] = {};
		Uid _uidsMisc = 0;
	};
}
