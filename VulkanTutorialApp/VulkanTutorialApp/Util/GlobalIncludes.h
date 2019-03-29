#pragma once

namespace Util
{
	class NullType {};

	template<typename Type> Type* CheckCast(NullType* ptr)
	{
		JE_Assert(dynamic_cast<Type*>(ptr) != nullptr);
		return static_cast<Type*>(ptr);
	}

	template<typename Type> const Type* CheckCast(const NullType* ptr)
	{
		JE_Assert(dynamic_cast<const Type*>(ptr) != nullptr);
		return static_cast<const Type*>(ptr);
	}
}

#include "Util/Singleton.h"