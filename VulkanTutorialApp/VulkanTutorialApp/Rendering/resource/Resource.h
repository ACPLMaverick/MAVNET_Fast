#pragma once

#include "ResourceCommon.h"

namespace Rendering
{
	class Resource
	{
	public:

		Resource();
		~Resource();

		JE_Inline ResourceCommon::Type GetType() const { return _type; }

	protected:

		ResourceCommon::Type _type;
	};
}