#pragma once

#include "Rendering/resource/Shader.h"

namespace Core
{
	class ManagerUid
	{
	public:

		ManagerUid() { }
		~ManagerUid() { }

		void Initialize();
		void Cleanup();

		UidShader ObtainUidForShader();
		UidMisc ObtainUidForMisc();

		void FreeUidForShader(UidShader uid);
		void FreeUidForMisc(UidMisc uid);

	private:

		UidShader _uidsShader = 0;
		UidMisc _uidsMisc = 0;
	};
}
