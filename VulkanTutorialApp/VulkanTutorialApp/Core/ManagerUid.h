#pragma once

#include "Rendering/resource/Shader.h"

namespace Core
{
	class ManagerUid
	{
	public:

		template<typename UidType>
		class UidCache
		{
		public:

			UidType Get()
			{
				++_currentUid;
				JE_Assert(_currentUid < std::numeric_limits<UidType>::max());
				return _currentUid;
			}

			void Free(UidType uid)
			{
				JE_TODO();	// TODO: Implement.
			}

		private:

			UidType _currentUid = std::numeric_limits<UidType>::max();

			friend class ManagerUid;
		};

	public:

		ManagerUid() { }
		~ManagerUid() { }

		void Initialize();
		void Cleanup();


		UidCache<UidShader> UidCacheShaders;
		UidCache<UidEntity> UidCacheEntities;
		UidCache<UidMisc> UidCacheMisc;
	};
}
