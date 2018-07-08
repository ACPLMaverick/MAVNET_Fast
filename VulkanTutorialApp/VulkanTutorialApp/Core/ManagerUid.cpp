#include "ManagerUid.h"


namespace Core
{
	void ManagerUid::Initialize()
	{
	}

	void ManagerUid::Cleanup()
	{
	}

	UidShader ManagerUid::ObtainUidForShader()
	{
		++_uidsShader;
		JE_Assert(_uidsShader <= Rendering::Shader::MAX_SHADER_COUNT);
		return _uidsShader;
	}

	UidMisc ManagerUid::ObtainUidForMisc()
	{
		++_uidsMisc;
		return _uidsMisc;
	}

	void ManagerUid::FreeUidForShader(UidShader uid)
	{
		// TODO: implement
		JE_PrintWarnLine("Freeing uids are not yet implemented!");
	}

	void ManagerUid::FreeUidForMisc(UidMisc uid)
	{
		// TODO: implement
		JE_PrintWarnLine("Freeing uids are not yet implemented!");
	}
}