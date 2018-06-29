#include "ManagerUid.h"


namespace Core
{
	void ManagerUid::Initialize()
	{
	}

	void ManagerUid::Cleanup()
	{
	}

	Uid ManagerUid::ObtainUidForShaderModule(Rendering::Shader::Stage stage)
	{
		++_uidsShaderModule[static_cast<uint8_t>(stage)];
		JE_Assert(_uidsShaderModule[static_cast<uint8_t>(stage)] <= Rendering::Shader::MAX_SHADER_COUNT_COMMON);
		return _uidsShaderModule[static_cast<uint8_t>(stage)];
	}

	Uid ManagerUid::ObtainUidForMisc()
	{
		++_uidsMisc;
		return _uidsMisc;
	}

	void ManagerUid::FreeUidForShaderModule(Rendering::Shader::Stage stage, Uid uid)
	{
		// TODO: implement
		JE_PrintWarnLine("Freeing uids are not yet implemented!");
	}

	void ManagerUid::FreeUidForMisc(Uid uid)
	{
		// TODO: implement
		JE_PrintWarnLine("Freeing uids are not yet implemented!");
	}
}